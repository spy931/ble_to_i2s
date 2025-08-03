/**
 * @file main.c
 * @brief Test application main file for BLE audio player and button control.
 *
 * This application initializes BLE, buttons, logging, and audio playback.
 * It handles button events to play/stop audio tracks and manages BLE
 * advertising.
 */

#include <stdint.h>
#include <string.h>

#include "app_button.h"
#include "app_error.h"
#include "app_timer.h"
#include "ble_handlers.h"
#include "ble_user.h"
#include "boards.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "sound_player.h"
#include "wav_db.h"

/**
 * @def PLAY1_BUTTON
 * @brief Button that will trigger the notification event for track 1.
 */
/**
 * @def PLAY2_BUTTON
 * @brief Button that will trigger the notification event for track 2.
 */
/**
 * @def STOP_BUTTON
 * @brief Button that will trigger the stop event.
 */
/**
 * @def BUTTON_DETECTION_DELAY
 * @brief Delay from a GPIOTE event until a button is reported as pushed (in
 * timer ticks).
 */
/**
 * @def DEAD_BEEF
 * @brief Value used as error code on stack dump, can be used to identify stack
 * location on stack unwind.
 */

#define PLAY1_BUTTON \
  BSP_BUTTON_0 /**< Button that will trigger the notification event*/
#define PLAY2_BUTTON BSP_BUTTON_1
#define STOP_BUTTON BSP_BUTTON_2
#define BUTTON_DETECTION_DELAY                                         \
  APP_TIMER_TICKS(50) /**< Delay from a GPIOTE event until a button is \
                         reported as pushed (in number of timer ticks). */

#define DEAD_BEEF                                                        \
  0xDEADBEEF /**< Value used as error code on stack dump, can be used to \
                identify stack location on stack unwind. */

/**@brief Function for assert macro callback.
 *
 * This function will be called in case of an assert in the SoftDevice.
 * On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t* p_file_name) {
  app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**
 * @brief Function for the LEDs initialization.
 *
 * Initializes all LEDs used by the application.
 */
static void leds_init(void) { bsp_board_init(BSP_INIT_LEDS); }

/**
 * @brief Function for the Timer initialization.
 *
 * Initializes the timer module.
 */
static void timers_init(void) {
  // Initialize timer module, making it use the scheduler
  ret_code_t err_code = app_timer_init();
  APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action) {
  ret_code_t err_code;

  switch (pin_no) {
    case PLAY1_BUTTON:
      if (button_action) {
        NRF_LOG_INFO("Button track 1 pressed");
        snd_cmd_write_handler(0, 0, 0x01);
      }
      break;

    case PLAY2_BUTTON:
      if (button_action) {
        NRF_LOG_INFO("Button track 2 pressed");
        snd_cmd_write_handler(0, 0, 0x02);
      }
      break;

    case STOP_BUTTON:
      if (button_action) {
        NRF_LOG_INFO("Stop button pressed");
        snd_cmd_write_handler(0, 0, 0x03);
      }
      break;

    default:
      APP_ERROR_HANDLER(pin_no);
      break;
  }
}

/**
 * @brief Function for initializing the button handler module.
 */
static void buttons_init(void) {
  ret_code_t err_code;

  // The array must be static because a pointer to it will be saved in the
  // button handler module.
  static app_button_cfg_t buttons[] = {
      {PLAY1_BUTTON, false, BUTTON_PULL, button_event_handler},
      {PLAY2_BUTTON, false, BUTTON_PULL, button_event_handler},
      {STOP_BUTTON, false, BUTTON_PULL, button_event_handler}};

  err_code =
      app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DETECTION_DELAY);
  APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for initializing logging.
 */
static void log_init(void) {
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**
 * @brief Function for initializing power management.
 */
static void power_management_init(void) {
  ret_code_t err_code;
  err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for handling the idle state (main loop).
 *
 * If there is no pending log operation, then sleep until the next event occurs.
 */
static void idle_state_handle(void) {
  if (NRF_LOG_PROCESS() == false) {
    nrf_pwr_mgmt_run();
  }
}

/**
 * @brief Function for application main entry.
 *
 * Initializes all modules, starts BLE advertising, and enters the main loop.
 *
 * @return int Unused.
 */
int main(void) {
  // Initialize.
  log_init();
  leds_init();
  timers_init();
  buttons_init();
  power_management_init();
  ble_stack_init();
  gap_params_init();
  gatt_init();
  services_init();
  advertising_init();
  conn_params_init();
  WAV_Init();
  SoundPlayer_Init();

  // Start execution.
  advertising_start();
  NRF_LOG_INFO("BLE service started");
  app_button_enable();

  // Enter main loop.
  for (;;) {
    idle_state_handle();
  }
}

/**
 * @}
 */
