/**
 * @file sound_player.c
 * @brief Sound player implementation using I2S for WAV playback.
 *
 * This file implements the sound player module, providing initialization,
 * WAV file playback, and I2S data handling.
 */

#include "sound_player.h"

#include <stdlib.h>
#include <string.h>

#include "ble_user.h"
#include "nrf_drv_common.h"
#include "nrf_drv_i2s.h"
#include "nrf_gpio.h"

#define I2S_DATA_BLOCKS_COUNT (8)
#define I2S_BUFFER_SIZE (I2S_DATA_BLOCKS_COUNT * sizeof(uint32_t))
#define I2S_TX_BUFFERS_COUNT (2)

#define SET_NEXT_ACTIVE_BUFF() \
  if (++active_buffer_index == I2S_TX_BUFFERS_COUNT) active_buffer_index = 0;

static uint32_t i2s_tx_buffers[I2S_TX_BUFFERS_COUNT][I2S_DATA_BLOCKS_COUNT];
static uint8_t active_buffer_index = 0;
static uint32_t data_size_to_play = 0;
static const char* data_to_play_ptr = 0;

static nrf_drv_i2s_config_t config = NRF_DRV_I2S_DEFAULT_CONFIG;

static ret_code_t SoundPlayer_SetNextDataBlock(void);
static ret_code_t SoundPlayer_SetChannels(uint16_t channels_num);
static ret_code_t SoundPlayer_SetSampleRate(uint32_t sample_rate);
static ret_code_t SoundPlayer_SetBitsPerSample(uint16_t bps);
static void data_handler(nrf_drv_i2s_buffers_t const* p_released,
                         uint32_t status);

/**
 * @brief Initialize the sound player and I2S buffers.
 *
 * @return NRF_SUCCESS on success, error code otherwise.
 */
ret_code_t SoundPlayer_Init() {
  memset(i2s_tx_buffers, 0, I2S_BUFFER_SIZE * I2S_TX_BUFFERS_COUNT);

  config.sdin_pin = I2S_SDIN_PIN;
  config.sdout_pin = I2S_SDOUT_PIN;
  config.mode = NRF_I2S_MODE_MASTER;

  return NRF_SUCCESS;
}

/**
 * @brief Open a WAV file for playback.
 *
 * Configures the I2S interface according to the WAV header.
 *
 * @param[in] wav_ptr Pointer to the WAV header structure.
 * @return NRF_SUCCESS on success, error code otherwise.
 */
ret_code_t SoundPlayer_OpenWAV(WavHeader_Struct* wav_ptr) {
  ret_code_t err_code = NRF_SUCCESS;

  err_code = SoundPlayer_SetChannels(wav_ptr->NumChannels);
  if (err_code == NRF_SUCCESS)
    err_code = SoundPlayer_SetSampleRate(wav_ptr->SampleRate);
  if (err_code == NRF_SUCCESS)
    err_code = SoundPlayer_SetBitsPerSample(wav_ptr->BitsPerSample);

  data_size_to_play = wav_ptr->DataSize;
  // data_size_to_play = 8;
  data_to_play_ptr = wav_ptr->data_ptr;
  // data_to_play_ptr = test_data;

  if (err_code == NRF_SUCCESS)
    err_code = nrf_drv_i2s_init(&config, data_handler);

  return err_code;
}

/**
 * @brief Start playback of the opened sound file.
 *
 * @return NRF_SUCCESS on success, error code otherwise.
 */
ret_code_t SoundPlayer_Play() {
  ret_code_t err_code = NRF_SUCCESS;

  err_code = SoundPlayer_SetNextDataBlock();

  if (err_code == NRF_SUCCESS) {
    nrf_drv_i2s_buffers_t const initial_buffers = {
        .p_tx_buffer = (uint32_t*)i2s_tx_buffers[active_buffer_index],
        .p_rx_buffer = 0,
    };
    err_code = nrf_drv_i2s_start(&initial_buffers, I2S_DATA_BLOCKS_COUNT, 0);
  }
}

/**
 * @brief Stop playback of the sound file.
 *
 * @return NRF_SUCCESS on success, error code otherwise.
 */
ret_code_t SoundPlayer_Stop() {
  nrf_drv_i2s_stop();
  nrf_drv_i2s_uninit();
  return acc_data_update(rand(), rand(), rand());
}

/**
 * @brief Set the next data block for I2S transmission.
 *
 * @return NRF_SUCCESS on success, error code otherwise.
 */
static ret_code_t SoundPlayer_SetNextDataBlock(void) {
  if (!data_size_to_play) return NRF_ERROR_DATA_SIZE;

  if (!data_to_play_ptr) return NRF_ERROR_NULL;

  memset(i2s_tx_buffers[active_buffer_index], 0, I2S_BUFFER_SIZE);
  if (data_size_to_play > I2S_BUFFER_SIZE) {
    memcpy(&i2s_tx_buffers[active_buffer_index], data_to_play_ptr,
           I2S_BUFFER_SIZE);
    data_size_to_play -= I2S_BUFFER_SIZE;
    data_to_play_ptr += I2S_BUFFER_SIZE;
  } else {
    memcpy(&i2s_tx_buffers[active_buffer_index], data_to_play_ptr,
           data_size_to_play);
    data_size_to_play = 0;
  }

  return NRF_SUCCESS;
}

/**
 * @brief Set the number of channels for I2S playback.
 *
 * @param[in] channels_num Number of channels (1 for mono, 2 for stereo).
 * @return NRF_SUCCESS on success, error code otherwise.
 */
ret_code_t SoundPlayer_SetChannels(uint16_t channels_num) {
  ret_code_t err_code = NRF_SUCCESS;

  if (channels_num == 1) {
    config.channels = NRF_I2S_CHANNELS_LEFT;
    config.format = I2S_CONFIG_FORMAT_FORMAT_I2S;
    config.alignment = I2S_CONFIG_ALIGN_ALIGN_LEFT;
  } else if (channels_num == 2) {
    config.channels = NRF_I2S_CHANNELS_STEREO;
    config.format = I2S_CONFIG_FORMAT_FORMAT_I2S;
    config.alignment = I2S_CONFIG_ALIGN_ALIGN_LEFT;
  } else
    err_code = NRF_ERROR_INVALID_PARAM;

  return err_code;
}

/**
 * @brief Set the sample rate for I2S playback.
 *
 * @param[in] sample_rate Sample rate in Hz.
 * @return NRF_SUCCESS on success, error code otherwise.
 */
ret_code_t SoundPlayer_SetSampleRate(uint32_t sample_rate) {
  ret_code_t err_code = NRF_SUCCESS;

  switch (sample_rate) {
    case 8000:
      config.mck_setup = NRF_I2S_MCK_32MDIV125;
      config.ratio = NRF_I2S_RATIO_32X;
      break;

    case 11025:
      config.mck_setup = NRF_I2S_MCK_32MDIV30;
      config.ratio = NRF_I2S_RATIO_96X;
      break;

    case 16000:
      config.mck_setup = NRF_I2S_MCK_32MDIV63;
      config.ratio = NRF_I2S_RATIO_32X;
      break;

    case 22050:
      config.mck_setup = NRF_I2S_MCK_32MDIV15;
      config.ratio = NRF_I2S_RATIO_96X;
      break;

    case 32000:
      config.mck_setup = NRF_I2S_MCK_32MDIV16;
      config.ratio = NRF_I2S_RATIO_64X;
      break;

    case 44100:
      config.mck_setup = NRF_I2S_MCK_32MDIV21;
      config.ratio = NRF_I2S_RATIO_96X;
      break;

    case 48000:
      config.mck_setup = NRF_I2S_MCK_32MDIV32;
      config.ratio = NRF_I2S_RATIO_64X;
      break;

    default:
      // For real we have to calculate DIV and RATIO for custom Bitrate
      // But i have no time to complete it now
      err_code = NRF_ERROR_INVALID_PARAM;
      break;
  }

  return err_code;
}

/**
 * @brief Set the bits per sample for I2S playback.
 *
 * @param[in] bps Bits per sample (8, 16, or 24).
 * @return NRF_SUCCESS on success, error code otherwise.
 */
ret_code_t SoundPlayer_SetBitsPerSample(uint16_t bps) {
  ret_code_t err_code = NRF_SUCCESS;

  switch (bps) {
    case 8:
      config.sample_width = I2S_CONFIG_SWIDTH_SWIDTH_8BIT;
      break;

    case 16:
      config.sample_width = I2S_CONFIG_SWIDTH_SWIDTH_16BIT;
      break;

    case 24:
      config.sample_width = I2S_CONFIG_SWIDTH_SWIDTH_24BIT;
      break;

    default:
      err_code = NRF_ERROR_INVALID_PARAM;
      break;
  }

  return err_code;
}

/**
 * @brief I2S data handler callback.
 *
 * Handles buffer release and sets up the next buffer for transmission.
 *
 * @param[in] p_released Pointer to the released I2S buffers.
 * @param[in] status     Status flags.
 */
static void data_handler(nrf_drv_i2s_buffers_t const* p_released,
                         uint32_t status) {
  uint32_t err_code = NRF_SUCCESS;

  ASSERT(p_released);

  // When the handler is called after the transfer has been stopped
  // (no next buffers are needed, only the used buffers are to be
  // released), there is nothing to do.
  if (!(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED)) {
    return;
  }

  if (p_released->p_tx_buffer) {
    SET_NEXT_ACTIVE_BUFF()
    err_code = SoundPlayer_SetNextDataBlock();

    if (err_code == NRF_SUCCESS) {
      nrf_drv_i2s_buffers_t const next_buffers = {
          .p_tx_buffer = (uint32_t*)i2s_tx_buffers[active_buffer_index],
          .p_rx_buffer = 0,
      };
      APP_ERROR_CHECK(nrf_drv_i2s_next_buffers_set(&next_buffers));
    } else {
      SoundPlayer_Stop();
    }
  }
}
