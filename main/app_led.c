#include "app_led.h"
#include "esp_log.h"

static const char *TAG = "app_led";

void app_led_set_color(const char *color) {
    ESP_LOGI(TAG, "Setting LED color to: %s", color);

    // TODO: actual GPIO / LED logic later
}