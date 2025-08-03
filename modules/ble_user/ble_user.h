/**
 * @file ble_user.h
 * @brief BLE user application and service initialization interface.
 */

#include <stdint.h>

/**
 * @brief Initialize GAP parameters (device name, appearance, connection params).
 */
void gap_params_init(void);

/**
 * @brief Initialize the GATT module.
 */
void gatt_init(void);

/**
 * @brief Initialize BLE advertising functionality.
 */
void advertising_init(void);

/**
 * @brief Initialize the connection parameters module.
 */
void conn_params_init(void);

/**
 * @brief Start BLE advertising.
 */
void advertising_start(void);

/**
 * @brief Initialize the BLE stack (SoftDevice and BLE event interrupt).
 */
void ble_stack_init(void);

/**
 * @brief Initialize BLE services used by the application.
 */
void services_init(void);

/**
 * @brief Update accelerometer data over BLE.
 *
 * @param[in] x_in X-axis value.
 * @param[in] y_in Y-axis value.
 * @param[in] z_in Z-axis value.
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t acc_data_update(uint16_t x_in, uint16_t y_in, uint16_t z_in);