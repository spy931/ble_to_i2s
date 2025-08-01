/**
 * @file wav_db.h
 * @brief WAV audio database structures and access functions.
 *
 * This header defines structures and functions for managing and accessing
 * a database of WAV audio files, including their headers and associated metadata.
 */

#ifndef _WAV_DB_H
#define _WAV_DB_H

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
  char* data_ptr;           /**< Pointer to audio data */
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


/**
 * @brief Initialize the WAV audio database.
 *
 * This function performs any necessary setup or initialization required
 * to use the WAV audio database, such as loading sound data or preparing
 * internal structures.
 *
 * @return 0 on success, non-zero on failure.
 */
uint8_t WAV_Init();

/**
 * @brief Get the number of sounds in the WAV database.
 * @param data_ptr Pointer to the data buffer containing the sound list.
 * @return Number of sounds found.
 */
uint16_t WAV_GetSoundList(Sound_Struct* data_ptr);

/**
 * @brief Retrieve a WAV sound by its name.
 * @param name Name of the sound.
 * @return Pointer to the WAV header structure, or NULL if not found.
 */
Sound_Struct* WAV_GetSoundByName(char* name);

/**
 * @brief Retrieve a WAV sound by its index in the database.
 * @param index Index of the sound.
 * @return Pointer to the WAV header structure, or NULL if not found.
 */
Sound_Struct* WAV_GetSoundByIndex(uint16_t index);

#endif  //_WAV_DB_H