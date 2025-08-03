/**
 * @file ble_handlers.c
 * @brief BLE command handler implementation for sound playback control.
 *
 * This file implements BLE command handling for sound playback,
 * including play and stop commands, and interaction with the sound player.
 */

#include "ble_handlers.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "sound_player.h"
#include "wav_db.h"

static snd_play_track(uint16_t track_num);

/**
 * @brief Handles sound command writes from BLE.
 *
 * @param[in] conn_handle BLE connection handle.
 * @param[in] p_snd       Pointer to the BLE sound service structure.
 * @param[in] cmd         Command byte (0x01/0x02: play track, 0x03: stop).
 */
void snd_cmd_write_handler(uint16_t conn_handle, ble_snd_t* p_snd,
                           uint8_t cmd) {
  switch (cmd) {
    case 0x01:
    case 0x02:
      snd_play_track(cmd);
      break;

    case 0x03:
      NRF_LOG_INFO("Stop playing");
      SoundPlayer_Stop();
      break;

    default:
      NRF_LOG_INFO("Unknown command");
      break;
  }
}

static Sound_Struct* snd_ptr = NULL;

/**
 * @brief Play the specified track by index.
 *
 * @param[in] track_num Track number to play (1-based index).
 */
static snd_play_track(uint16_t track_num) {
  NRF_LOG_INFO("Playing Sound %d", track_num);
  snd_ptr = WAV_GetSoundByIndex(track_num - 1);
  SoundPlayer_OpenWAV(&(snd_ptr->header));
  SoundPlayer_Play();
}
