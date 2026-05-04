
#include "app_led.h"
#include "app_wifi.h"
#include "nvs_flash.h" // Non-volatile storage (persists w/ power off).

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    app_led_init();
    app_wifi_start();
}