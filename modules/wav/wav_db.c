/**
 * @file wav_db.c
 * @brief WAV sound database implementation for static sound library.
 *
 * This module provides initialization and access functions for a static
 * WAV sound library, including validation of WAV headers.
 */

#include "wav_db.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "sound1.h"
#include "sound2.h"

#define SOUNDS_COUNT (2)

static Sound_Struct sounds_library[SOUNDS_COUNT];

// Checks if the provided WAV header data is valid according to expected format
// constraints.
static ret_code_t ValidWavData(WavHeader_Struct* Wav);

/**
 * @brief Initialize the WAV sound library.
 *
 * Loads static WAV data into the sound library and validates headers.
 *
 * @return NRF_SUCCESS on success, error code otherwise.
 */
ret_code_t WAV_Init() {
  // At real life we will get data from SD-card
  // But at this example we just initialize module with 2 static sounds from
  // arrays
  strncpy(sounds_library[0].name, "Sound1", MAX_FILENAME_SIZE);
  memcpy(&(sounds_library[0].header), WavFile1, WAV_HEADER_SIZE);
  sounds_library[0].header.data_ptr = &WavFile1[WAV_HEADER_SIZE];

  strncpy(sounds_library[1].name, "Sound2", MAX_FILENAME_SIZE);
  memcpy(&(sounds_library[1].header), WavFile2, WAV_HEADER_SIZE);
  sounds_library[1].header.data_ptr = &WavFile2[WAV_HEADER_SIZE];

  for (uint8_t i = 0; i < SOUNDS_COUNT; i++) {
    uint8_t res = ValidWavData(&sounds_library[i].header);
    if (res != NRF_SUCCESS) return res;
    NRF_LOG_INFO("Track %d init successfully", i);
  }

  return NRF_SUCCESS;
}

/**
 * @brief Get the list of available sounds.
 *
 * @param[out] data_ptr Pointer to where the sound list will be returned.
 * @return Number of sounds in the library.
 */
uint16_t WAV_GetSoundList(Sound_Struct* data_ptr) {
  if (data_ptr) {
    data_ptr = sounds_library;
    return SOUNDS_COUNT;
  }
  return 0;
}

/**
 * @brief Get a sound by its name.
 *
 * @param[in] name Name of the sound.
 * @return Pointer to the Sound_Struct if found, NULL otherwise.
 */
Sound_Struct* WAV_GetSoundByName(const char* name) {
  for (uint8_t i = 0; i < SOUNDS_COUNT; i++) {
    if (strcmp(name, sounds_library[i].name) == 0) return &sounds_library[i];
  }
  return 0;
}

/**
 * @brief Get a sound by its index.
 *
 * @param[in] index Index of the sound.
 * @return Pointer to the Sound_Struct.
 */
Sound_Struct* WAV_GetSoundByIndex(uint16_t index) {
  return &sounds_library[index];
}

/**
 * @brief Validate the WAV header data.
 *
 * Checks if the WAV header matches expected format and constraints.
 *
 * @param[in] Wav Pointer to the WAV header structure.
 * @return NRF_SUCCESS if valid, NRF_ERROR_INVALID_DATA otherwise.
 */
static ret_code_t ValidWavData(WavHeader_Struct* Wav) {
  if (memcmp(Wav->RiffSectionID, "RIFF", 4) != 0) {
    NRF_LOG_ERROR("Invlaid data - Not RIFF format");
    return NRF_ERROR_INVALID_DATA;
  }
  if (memcmp(Wav->RiffFormat, "WAVE", 4) != 0) {
    NRF_LOG_ERROR("Invlaid data - Not Wave file");
    return NRF_ERROR_INVALID_DATA;
  }
  if (memcmp(Wav->FormatSectionID, "fmt", 3) != 0) {
    NRF_LOG_ERROR("Invlaid data - No format section found");
    return NRF_ERROR_INVALID_DATA;
  }
  if (memcmp(Wav->DataSectionID, "data", 4) != 0) {
    NRF_LOG_ERROR("Invlaid data - data section not found");
    return NRF_ERROR_INVALID_DATA;
  }
  if (Wav->FormatID != 1) {
    NRF_LOG_ERROR("Invlaid data - format Id must be 1");
    return NRF_ERROR_INVALID_DATA;
  }
  if (Wav->FormatSize != 16) {
    NRF_LOG_ERROR("Invlaid data - format section size must be 16");
    return NRF_ERROR_INVALID_DATA;
  }
  if ((Wav->NumChannels != 1) & (Wav->NumChannels != 2)) {
    NRF_LOG_ERROR("Invlaid data - only mono or stereo permitted");
    return NRF_ERROR_INVALID_DATA;
  }
  if (Wav->SampleRate > 48000) {
    NRF_LOG_ERROR("Invlaid data - Sample rate cannot be greater than 48000");
    return NRF_ERROR_INVALID_DATA;
  }
  if ((Wav->BitsPerSample != 8) & (Wav->BitsPerSample != 16)) {
    NRF_LOG_ERROR("Invlaid data - Only 8 or 16 bits per sample permitted");
    return NRF_ERROR_INVALID_DATA;
  }

  return NRF_SUCCESS;
}
