/**
 * @file ble_handlers.h
 * @brief BLE command handler interface for sound playback control.
 */

#include <stdint.h>
#include "ble_snd.h"

/**
 * @brief Handles sound command writes from BLE.
 *
 * @param[in] conn_handle BLE connection handle.
 * @param[in] p_snd       Pointer to the BLE sound service structure.
 * @param[in] cmd         Command byte (0x01/0x02: play track, 0x03: stop).
 */
void snd_cmd_write_handler(uint16_t conn_handle, ble_snd_t* p_snd, uint8_t cmd);