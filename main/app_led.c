#include "app_led.h"
#include "esp_log.h"
#include "led_strip.h"

static const char *TAG = "app_led";

#define RGB_LED_GPIO 38
#define RGB_LED_COUNT 1

static led_strip_handle_t s_led_strip = NULL;

void app_led_init(void) {
    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_LED_GPIO,
        .max_leds = RGB_LED_COUNT,
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000,
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(
        &strip_config,
        &rmt_config,
        &s_led_strip
    ));

    led_strip_clear(s_led_strip);

    ESP_LOGI(TAG, "RGB LED initialized");
}

void app_led_set_color(uint8_t r, uint8_t g, uint8_t b) {
    ESP_LOGI(TAG, "Setting LED color to R:%d G:%d B:%d", r, g, b);

    if (s_led_strip == NULL) {
        ESP_LOGE(TAG, "LED strip not initialized");
        return;
    }

    ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, r, g, b));
    ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}