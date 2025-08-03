/**
 * @file sound_player.h
 * @brief Sound player interface for WAV audio playback.
 */

#ifndef _SOUND_PLAYER_
#define _SOUND_PLAYER_

#include "wav_db_types.h"
#include "sdk_errors.h"

/**
 * @brief Initializes the sound player module.
 * @return NRF_SUCCESS on success, error code otherwise.
 */
ret_code_t SoundPlayer_Init();

/**
 * @brief Opens a WAV file for playback.
 * @param wav_ptr Pointer to the WAV header structure.
 * @return 0 on success, error code otherwise.
 */
ret_code_t SoundPlayer_OpenWAV(WavHeader_Struct* wav_ptr);

/**
 * @brief Starts playback of the opened sound file.
 * @return 0 on success, error code otherwise.
 */
ret_code_t SoundPlayer_Play();

/**
 * @brief Stops playback of the sound file.
 * @return 0 on success, error code otherwise.
 */
ret_code_t SoundPlayer_Stop();

#endif //_SOUND_PLAYER_