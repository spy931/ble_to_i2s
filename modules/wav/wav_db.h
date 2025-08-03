/**
 * @file wav_db.h
 * @brief WAV audio database structures and access functions.
 *
 * This header defines structures and functions for managing and accessing
 * a database of WAV audio files, including their headers and associated metadata.
 */

#ifndef _WAV_DB_H
#define _WAV_DB_H

#include "wav_db_types.h"
#include "sdk_errors.h"

/**
 * @brief Initialize the WAV audio database.
 *
 * This function performs any necessary setup or initialization required
 * to use the WAV audio database, such as loading sound data or preparing
 * internal structures.
 *
 * @return 0 on success, non-zero on failure.
 */
ret_code_t WAV_Init();

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
Sound_Struct* WAV_GetSoundByName(const char* name);

/**
 * @brief Retrieve a WAV sound by its index in the database.
 * @param index Index of the sound.
 * @return Pointer to the WAV header structure, or NULL if not found.
 */
Sound_Struct* WAV_GetSoundByIndex(uint16_t index);

#endif  //_WAV_DB_H