#include "wav_db.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sound1.h"
#include "sound2.h"

#define SOUNDS_COUNT (2)

static Sound_Struct sounds_library[SOUNDS_COUNT];

// Checks if the provided WAV header data is valid according to expected format constraints.
static ret_code_t ValidWavData(WavHeader_Struct* Wav);

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
    printf("Track %d init successfully\r\n", i);
  }

  return NRF_SUCCESS;
}

uint16_t WAV_GetSoundList(Sound_Struct* data_ptr) {
  if (data_ptr) {
    data_ptr = sounds_library;
    return SOUNDS_COUNT;
  }
  return 0;
}

Sound_Struct* WAV_GetSoundByName(const char* name) {
  for (uint8_t i = 0; i < SOUNDS_COUNT; i++) {
    if (strcmp(name, sounds_library[i].name) == 0) 
      return &sounds_library[i];
  }
  return 0;
}

Sound_Struct* WAV_GetSoundByIndex(uint16_t index) {
  return &sounds_library[index];
}

static ret_code_t ValidWavData(WavHeader_Struct* Wav) {
  if (memcmp(Wav->RiffSectionID, "RIFF", 4) != 0) {
    printf("Invlaid data - Not RIFF format\r\n");
    return NRF_ERROR_INVALID_DATA;
  }
  if (memcmp(Wav->RiffFormat, "WAVE", 4) != 0) {
    printf("Invlaid data - Not Wave file\r\n");
    return NRF_ERROR_INVALID_DATA;
  }
  if (memcmp(Wav->FormatSectionID, "fmt", 3) != 0) {
    printf("Invlaid data - No format section found\r\n");
    return NRF_ERROR_INVALID_DATA;
  }
  if (memcmp(Wav->DataSectionID, "data", 4) != 0) {
    printf("Invlaid data - data section not found\r\n");
    return NRF_ERROR_INVALID_DATA;
  }
  if (Wav->FormatID != 1) {
    printf("Invlaid data - format Id must be 1\r\n");
    return NRF_ERROR_INVALID_DATA;
  }
  if (Wav->FormatSize != 16) {
    printf("Invlaid data - format section size must be 16.\r\n");
    return NRF_ERROR_INVALID_DATA;
  }
  if ((Wav->NumChannels != 1) & (Wav->NumChannels != 2)) {
    printf("Invlaid data - only mono or stereo permitted.\r\n");
    return NRF_ERROR_INVALID_DATA;
  }
  if (Wav->SampleRate > 48000) {
    printf("Invlaid data - Sample rate cannot be greater than 48000\r\n");
    return NRF_ERROR_INVALID_DATA;
  }
  if ((Wav->BitsPerSample != 8) & (Wav->BitsPerSample != 16)) {
    printf("Invlaid data - Only 8 or 16 bits per sample permitted.\r\n");
    return NRF_ERROR_INVALID_DATA;
  }

  return NRF_SUCCESS;
}
