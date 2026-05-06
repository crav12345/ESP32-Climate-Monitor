
#include "app_bme280.h"
#include "app_led.h"
#include "app_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h" // Non-volatile storage (persists w/ power off).

void app_main(void) {
  ESP_ERROR_CHECK(nvs_flash_init());
  app_led_init();
  app_wifi_start();

  ESP_ERROR_CHECK(app_bme280_init());

  vTaskDelay(pdMS_TO_TICKS(1000));

  xTaskCreate(app_bme280_task, "app_bme280_task", 4096, NULL, 5, NULL);
}