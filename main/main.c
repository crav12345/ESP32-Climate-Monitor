
#include "app_led.h"
#include "app_wifi.h"
#include "app_i2c_scan.h"
#include "nvs_flash.h" // Non-volatile storage (persists w/ power off).
#include "app_bme280.h"

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    app_led_init();
    app_wifi_start();
    app_i2c_scan();
    app_bme280_start();
}