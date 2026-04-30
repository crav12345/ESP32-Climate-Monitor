#include "app_wifi.h"

#include "esp_wifi.h" // Wifi system.
#include "esp_netif.h" // Network interface system.
#include "esp_netif_ip_addr.h"
#include "esp_log.h" // Log system
#include "esp_event.h" // Event loop
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

static const char *TAG = "app_wifi";
static int s_retry_count = 0;

#define WIFI_SSID "The WiFi Monster"
#define WIFI_PASS "WhoWasInParis"
#define FAST_WIFI_RETRY_COUNT 10
#define FAST_WIFI_RETRY_DELAY_MS 1000
#define SLOW_WIFI_RETRY_DELAY_MS 30000

static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *event =
            (wifi_event_sta_disconnected_t *)event_data;

        s_retry_count++;

        int retry_delay_ms = s_retry_count <= FAST_WIFI_RETRY_COUNT
            ? FAST_WIFI_RETRY_DELAY_MS
            : SLOW_WIFI_RETRY_DELAY_MS;

        ESP_LOGI(
            TAG,
            "Disconnected. Reason: %d. Retry %d. Next retry in %d ms...",
            event->reason,
            s_retry_count,
            retry_delay_ms
        );

        vTaskDelay(pdMS_TO_TICKS(retry_delay_ms));
        esp_wifi_connect();
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // Cast to expected type so we can print.
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        s_retry_count = 0;
        ESP_LOGI(TAG, "WiFi connected. IP address: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void app_wifi_start(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    
    // Register event callbacks.
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // Wifi events. Wifi connection != network connection.
    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL
    ));
    // Network events. Do we actually have a usable network connection?
    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL
    ));

    // Default network interface to put the chip in 'station mode' to connect to wifi.
    esp_netif_create_default_wifi_sta();
    
    // Initialize the wifi driver.
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Station means connect to router. Access point means act like a router. We can be both, but here we want station.
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); // Wifi driver knows it's a client now.

    // Tell wifi driver which router to conenct to.
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Start wifi system now that it's configured.
    ESP_ERROR_CHECK(esp_wifi_start());

    // Actually start the connection attempt.
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "WiFi connection started");
}