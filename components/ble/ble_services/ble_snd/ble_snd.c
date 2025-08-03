/**
 * @file ble_snd.c
 * @brief BLE Sound Service implementation.
 *
 * This file implements the BLE service for sound command handling,
 * including initialization and BLE event processing.
 */

#include "ble_snd.h"

#include "ble_srv_common.h"
#include "sdk_common.h"

/**
 * @brief Function for handling the Write event on the Sound Command characteristic.
 *
 * @param[in] p_lbs      Sound Service structure.
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */
static void on_write(ble_snd_t* p_lbs, ble_evt_t const* p_ble_evt) {
  ble_gatts_evt_write_t const* p_evt_write =
      &p_ble_evt->evt.gatts_evt.params.write;

  if ((p_evt_write->handle == p_lbs->cmd_char_handles.value_handle) &&
      (p_evt_write->len == 1) && (p_lbs->cmd_write_handler != NULL)) {
    p_lbs->cmd_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_lbs,
                             p_evt_write->data[0]);
  }
}

/**
 * @brief Function for handling BLE events for the Sound Service.
 *
 * @param[in] p_ble_evt BLE event received.
 * @param[in] p_context Pointer to the Sound Service structure.
 */
void ble_snd_on_ble_evt(ble_evt_t const* p_ble_evt, void* p_context) {
  ble_snd_t* p_lbs = (ble_snd_t*)p_context;

  switch (p_ble_evt->header.evt_id) {
    case BLE_GATTS_EVT_WRITE:
      on_write(p_lbs, p_ble_evt);
      break;

    default:
      // No implementation needed.
      break;
  }
}

/**
 * @brief Function for initializing the Sound Service.
 *
 * Adds the Sound Service and its command characteristic to the BLE stack.
 *
 * @param[in] p_lbs      Pointer to the Sound Service structure.
 * @param[in] p_lbs_init Pointer to the initialization structure.
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_snd_init(ble_snd_t* p_lbs, const ble_snd_init_t* p_lbs_init) {
  uint32_t err_code;
  ble_uuid_t ble_uuid;
  ble_add_char_params_t add_char_params;

  // Initialize service structure.
  p_lbs->cmd_write_handler = p_lbs_init->cmd_write_handler;

  // Add service.
  ble_uuid128_t base_uuid = {LBS_UUID_BASE};
  err_code = sd_ble_uuid_vs_add(&base_uuid, &p_lbs->uuid_type);
  VERIFY_SUCCESS(err_code);

  ble_uuid.type = p_lbs->uuid_type;
  ble_uuid.uuid = LBS_UUID_SOUND_SERVICE;

  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid,
                                      &p_lbs->service_handle);
  VERIFY_SUCCESS(err_code);

  // Add Sound CMD characteristic.
  memset(&add_char_params, 0, sizeof(add_char_params));
  add_char_params.uuid = LBS_UUID_SOUND_CHAR;
  add_char_params.uuid_type = p_lbs->uuid_type;
  add_char_params.init_len = sizeof(uint8_t);
  add_char_params.max_len = sizeof(uint8_t);
  add_char_params.char_props.read = 0;
  add_char_params.char_props.write = 1;

  add_char_params.read_access = SEC_OPEN;
  add_char_params.write_access = SEC_OPEN;

  return characteristic_add(p_lbs->service_handle, &add_char_params,
                            &p_lbs->cmd_char_handles);
}