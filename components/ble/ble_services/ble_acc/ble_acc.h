/**
 * @file ble_acc.h
 * @brief BLE Accelerometer Service interface.
 *
 * This file defines the BLE service for accelerometer data transfer.
 */

#ifndef BLE_ACC_H__
#define BLE_ACC_H__

#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @def LBS_UUID_BASE
 * @brief 128-bit base UUID for the custom BLE Accelerometer Service.
 */
#define LBS_UUID_BASE                                                          \
  { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,                            \
    0xDE, 0xEF, 0x12, 0x14, 0x00, 0x00, 0x00, 0x00 }

/**
 * @def LBS_UUID_ACCELEROMETER_SERVICE
 * @brief 16-bit UUID for the Accelerometer Service.
 */
#define LBS_UUID_ACCELEROMETER_SERVICE 0x2669

/**
 * @def LBS_UUID_ACCELEROMETER_CHAR_XYZ
 * @brief 16-bit UUID for the Accelerometer XYZ Characteristic.
 */
#define LBS_UUID_ACCELEROMETER_CHAR_XYZ 0x2670

/**
 * @brief Forward declaration of the ble_acc_s type.
 */
typedef struct ble_acc_s ble_acc_t;

/**
 * @brief Structure for the BLE Accelerometer Service.
 */
struct ble_acc_s
{
    uint16_t service_handle;                  /**< Handle of Accelerometer Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t xyz_char_handles;/**< Handles related to the XYZ characteristic. */
    uint8_t uuid_type;                        /**< UUID type assigned for the service. */
};

/**
 * @brief Function for initializing the Accelerometer Service.
 *
 * @param[in] p_acc Pointer to the accelerometer service structure.
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_acc_init(ble_acc_t* p_acc);

/**
 * @brief Function for updating and notifying new accelerometer data.
 *
 * @param[in] conn_handle Connection handle.
 * @param[in] p_acc Pointer to the accelerometer service structure.
 * @param[in] x_in X-axis value.
 * @param[in] y_in Y-axis value.
 * @param[in] z_in Z-axis value.
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_acc_on_data_change(uint16_t conn_handle,
                                ble_acc_t* p_acc,
                                uint16_t x_in,
                                uint16_t y_in,
                                uint16_t z_in);

#ifdef __cplusplus
}
#endif

#endif // ble_ACC_H__

/** @} */
