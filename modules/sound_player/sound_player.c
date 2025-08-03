#include "sound_player.h"
#include "nrf_drv_i2s.h"
#include "nrf_drv_common.h"
#include "nrf_gpio.h"

#define I2S_BUFFER_SIZE           (256)
#define I2S_TX_BUFFERS_COUNT      (2)

static uint16_t i2s_tx_buffers[I2S_TX_BUFFERS_COUNT][I2S_BUFFER_SIZE];
static uint8_t  active_buffer_index = 0;
static uint8_t  data_size_to_play = 0;
static char*    data_to_play_ptr = 0;

static nrf_drv_i2s_config_t config = NRF_DRV_I2S_DEFAULT_CONFIG;

static ret_code_t SoundPlayer_SetChannels(uint16_t channels_num);
static ret_code_t SoundPlayer_SetSampleRate(uint32_t sample_rate);
static ret_code_t SoundPlayer_SetBitsPerSample(uint16_t bps);
static void data_handler(nrf_drv_i2s_buffers_t const * p_released,
                          uint32_t                      status);
                       

ret_code_t SoundPlayer_Init() {
    memset(i2s_tx_buffers, 0, I2S_BUFFER_SIZE*I2S_TX_BUFFERS_COUNT);

    config.sdin_pin  = I2S_SDIN_PIN;
    config.sdout_pin = I2S_SDOUT_PIN;
    config.mode = NRF_I2S_MODE_MASTER;
    config.format = I2S_CONFIG_FORMAT_FORMAT_Aligned;
    config.alignment = I2S_CONFIG_ALIGN_ALIGN_Left;

    return NRF_SUCCESS;
}

ret_code_t SoundPlayer_OpenWAV(WavHeader_Struct* wav_ptr){
    ret_code_t err_code = NRF_SUCCESS;

    err_code = SoundPlayer_SetChannels(wav_ptr->NumChannels);
    if(err_code == NRF_SUCCESS) 
        err_code = SoundPlayer_SetSampleRate(wav_ptr->SampleRate);
    if(err_code == NRF_SUCCESS) 
        err_code = SoundPlayer_SetBitsPerSample(wav_ptr->BitsPerSample);
    
    data_size_to_play = wav_ptr->DataSize;
    data_to_play_ptr = wav_ptr->data_ptr;

    if(err_code == NRF_SUCCESS)
        err_code = nrf_drv_i2s_init(&config, data_handler);

    return err_code;
}

ret_code_t SoundPlayer_Close(){}

ret_code_t SoundPlayer_Play(){
    ret_code_t err_code = NRF_SUCCESS;

    //err_code = nrf_drv_i2s_start(&initial_buffers, I2S_DATA_BLOCK_WORDS, 0);

}

ret_code_t SoundPlayer_Stop(){}

ret_code_t SoundPlayer_SetNextDataBlock(void){
  
  if(!data_size_to_play) return NRF_ERROR_DATA_SIZE;

  if(!data_to_play_ptr) return NRF_ERROR_NULL;

  if(data_size_to_play > I2S_BUFFER_SIZE){
      memcpy(&i2s_tx_buffers[active_buffer_index][0], data_to_play_ptr, I2S_BUFFER_SIZE);
      data_size_to_play -= I2S_BUFFER_SIZE;
  }
  else{
      memcpy(&i2s_tx_buffers[active_buffer_index][0], data_to_play_ptr, data_size_to_play);
      data_size_to_play = 0;
  }
  
  if (++active_buffer_index == I2S_TX_BUFFERS_COUNT) //Control overflow
    active_buffer_index = 0;
}

ret_code_t SoundPlayer_SetChannels(uint16_t channels_num){
    ret_code_t err_code = NRF_SUCCESS;

    if(channels_num == 1)
      config.channels  = NRF_I2S_CHANNELS_LEFT;
    else if(channels_num == 2)
      config.channels  = NRF_I2S_CHANNELS_STEREO;
    else
      err_code = NRF_ERROR_INVALID_PARAM;

    return err_code;
}

ret_code_t SoundPlayer_SetSampleRate(uint32_t sample_rate){
  ret_code_t err_code = NRF_SUCCESS;

  switch (sample_rate){
    case 8000:
      config.mck_setup = NRF_I2S_MCK_32MDIV125;
      config.ratio     = NRF_I2S_RATIO_32X;
      break;

    case 11025:
      config.mck_setup = NRF_I2S_MCK_32MDIV30;
      config.ratio     = NRF_I2S_RATIO_96X;
      break;

    case 16000:
      config.mck_setup = NRF_I2S_MCK_32MDIV21;
      config.ratio     = NRF_I2S_RATIO_96X;
      break;
      
    case 22050:
      config.mck_setup = NRF_I2S_MCK_32MDIV15;
      config.ratio     = NRF_I2S_RATIO_96X;
      break;

    case 32000:
      config.mck_setup = NRF_I2S_MCK_32MDIV8;
      config.ratio     = NRF_I2S_RATIO_256X;
      break;
    
    case 44100:
      config.mck_setup = NRF_I2S_MCK_32MDIV15;
      config.ratio     = NRF_I2S_RATIO_48X;
      break;
    
    case 48000:
      config.mck_setup = NRF_I2S_MCK_32MDIV21;
      config.ratio     = NRF_I2S_RATIO_32X;
      break;
    
    default:
      //For real we have to calculate DIV and RATIO for custom Bitrate
      //But i have no time to complete it now
      err_code = NRF_ERROR_INVALID_PARAM;
      break;
  }

  return err_code;
}

ret_code_t SoundPlayer_SetBitsPerSample(uint16_t bps){
    ret_code_t err_code = NRF_SUCCESS;

    switch (bps){
      case 8:
        config.sample_width = I2S_CONFIG_SWIDTH_SWIDTH_8Bit;
        break;
      
      case 16:
        config.sample_width = I2S_CONFIG_SWIDTH_SWIDTH_16Bit;
        break;
      
      case 24:
        config.sample_width = I2S_CONFIG_SWIDTH_SWIDTH_24Bit;
        break;

      default:
        err_code = NRF_ERROR_INVALID_PARAM;
        break;
    }

    return err_code;
}

static void data_handler(nrf_drv_i2s_buffers_t const * p_released,
                         uint32_t                      status)
{
    //// 'nrf_drv_i2s_next_buffers_set' is called directly from the handler
    //// each time next buffers are requested, so data corruption is not
    //// expected.
    //ASSERT(p_released);

    //// When the handler is called after the transfer has been stopped
    //// (no next buffers are needed, only the used buffers are to be
    //// released), there is nothing to do.
    //if (!(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED))
    //{
    //    return;
    //}

    //// First call of this handler occurs right after the transfer is started.
    //// No data has been transferred yet at this point, so there is nothing to
    //// check. Only the buffers for the next part of the transfer should be
    //// provided.
    //if (!p_released->p_rx_buffer)
    //{
    //    nrf_drv_i2s_buffers_t const next_buffers = {
    //        .p_rx_buffer = m_buffer_rx[1],
    //        .p_tx_buffer = m_buffer_tx[1],
    //    };
    //    APP_ERROR_CHECK(nrf_drv_i2s_next_buffers_set(&next_buffers));

    //    mp_block_to_fill = m_buffer_tx[1];
    //}
    //else
    //{
    //    mp_block_to_check = p_released->p_rx_buffer;
    //    // The driver has just finished accessing the buffers pointed by
    //    // 'p_released'. They can be used for the next part of the transfer
    //    // that will be scheduled now.
    //    APP_ERROR_CHECK(nrf_drv_i2s_next_buffers_set(p_released));

    //    // The pointer needs to be typecasted here, so that it is possible to
    //    // modify the content it is pointing to (it is marked in the structure
    //    // as pointing to constant data because the driver is not supposed to
    //    // modify the provided data).
    //    mp_block_to_fill = (uint32_t *)p_released->p_tx_buffer;
    //}
}

