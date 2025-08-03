#ifndef _WAV_DB_TYPES_H
#define _WAV_DB_TYPES_H

#include "stdint.h"

#define  WAV_HEADER_SIZE      (44)
#define  MAX_FILENAME_SIZE    (16)

/**
 * @brief Structure representing the header of a WAV audio file.
 *
 * This structure contains all the necessary fields to describe the format,
 * size, and data location of a standard uncompressed PCM WAV file.
 */
typedef struct {
  char RiffSectionID[4];    /**< Letters "RIFF" */
  uint32_t Size;            /**< Filesize minus 8 bytes */
  char RiffFormat[4];       /**< Letters "WAVE" */
  // Format Section
  char FormatSectionID[4];  /**< Letters "fmt " */
  uint32_t FormatSize;      /**< Size of format section minus 8 bytes */
  uint16_t FormatID;        /**< 1 = uncompressed PCM */
  uint16_t NumChannels;     /**< 1 = mono, 2 = stereo */
  uint32_t SampleRate;      /**< Sample rate (e.g., 44100, 16000, 8000) */
  uint32_t ByteRate;        /**< SampleRate * Channels * (BitsPerSample/8) */
  uint16_t BlockAlign;      /**< Channels * (BitsPerSample/8) */
  uint16_t BitsPerSample;   /**< Bits per sample (8, 16, 24, or 32) */
  // Data Section
  char DataSectionID[4];    /**< Letters "data" */
  uint32_t DataSize;        /**< Size of the data that follows */
  const char* data_ptr;           /**< Pointer to audio data */
} WavHeader_Struct;

/**
 * @brief Structure representing a named sound in the database.
 *
 * Associates a name with a WAV header structure.
 */
typedef struct {
  char name[MAX_FILENAME_SIZE]; /**< Name of the sound (null-terminated string) */
  WavHeader_Struct header;      /**< WAV header and data for the sound */
} Sound_Struct;

#endif //_WAV_DB_TYPES_H
