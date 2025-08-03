/**
 * @file ble_acc.c
 * @brief BLE Accelerometer Service implementation.
 *
 * This file implements the BLE service for accelerometer data transfer,
 * including initialization and characteristic value updates.
 */

#include "ble_acc.h"
#include "ble_srv_common.h"
#include "sdk_common.h"

#define XYZ_CHAR_SIZE 6

/**
 * @brief Packed structure for accelerometer XYZ data.
 */
typedef union {
  struct __attribute__((packed)) {
    uint16_t x;
    uint16_t y;
    uint16_t z;
  } values;
  uint8_t bytes[6];
} ble_acc_packed;

/**
 * @brief Function for initializing the BLE Accelerometer Service.
 *
 * Adds the Accelerometer Service and its XYZ characteristic to the BLE stack.
 *
 * @param[in] p_acc Pointer to the accelerometer service structure.
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_acc_init(ble_acc_t* p_acc) {
  uint32_t err_code;
  ble_uuid_t ble_uuid;
  ble_add_char_params_t add_char_params;

  ble_uuid128_t base_uuid = {LBS_UUID_BASE};
  err_code = sd_ble_uuid_vs_add(&base_uuid, &p_acc->uuid_type);
  VERIFY_SUCCESS(err_code);

  ble_uuid.type = p_acc->uuid_type;
  ble_uuid.uuid = LBS_UUID_ACCELEROMETER_SERVICE;

  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid,
                                      &p_acc->service_handle);
  VERIFY_SUCCESS(err_code);

  memset(&add_char_params, 0, sizeof(add_char_params));
  add_char_params.uuid = LBS_UUID_ACCELEROMETER_CHAR_XYZ;
  add_char_params.uuid_type = p_acc->uuid_type;
  add_char_params.init_len = XYZ_CHAR_SIZE;
  add_char_params.max_len = XYZ_CHAR_SIZE;
  add_char_params.char_props.read = 1;
  // add_char_params.char_props.notify = 1;

  add_char_params.read_access = SEC_OPEN;
  add_char_params.cccd_write_access = SEC_OPEN;

  return characteristic_add(p_acc->service_handle, &add_char_params,
                            &p_acc->xyz_char_handles);
}

/**
 * @brief Function for updating the accelerometer data characteristic.
 *
 * Packs the X, Y, and Z values and updates the characteristic value in the GATT database.
 *
 * @param[in] conn_handle Connection handle.
 * @param[in] p_acc Pointer to the accelerometer service structure.
 * @param[in] x_in X-axis value.
 * @param[in] y_in Y-axis value.
 * @param[in] z_in Z-axis value.
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_acc_on_data_change(uint16_t conn_handle, ble_acc_t* p_acc,
                                uint16_t x_in, uint16_t y_in, uint16_t z_in) {
  ble_acc_packed packed_data;
  packed_data.values.x = x_in;
  packed_data.values.y = y_in;
  packed_data.values.z = z_in;

  ble_gatts_value_t gatts_value = {
      .len = XYZ_CHAR_SIZE, .offset = 0, .p_value = packed_data.bytes};

  return sd_ble_gatts_value_set(
      conn_handle, p_acc->xyz_char_handles.value_handle, &gatts_value);
}
