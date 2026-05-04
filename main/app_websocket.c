#include "app_websocket.h"
#include "esp_websocket_client.h"
#include "esp_log.h"
#include <inttypes.h>

static const char *TAG = "app_websocket";

#define WEBSOCKET_URL "wss://climate-monitor-server-397967553683.us-east1.run.app/"

extern const uint8_t gts_root_r1_pem_start[] asm("_binary_gts_root_r1_pem_start");
extern const uint8_t gts_root_r1_pem_end[] asm("_binary_gts_root_r1_pem_end");

static void websocket_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
) {
    switch (event_id)
    {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WebSocket connected");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "WebSocket disconnected");
            break;
        case WEBSOCKET_EVENT_DATA:
            esp_websocket_event_data_t *data =
                (esp_websocket_event_data_t *)event_data;
            ESP_LOGI(TAG, "WebSocket data received: %.*s",
                data->data_len,
                (char *)data->data_ptr);
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGE(TAG, "WebSocket error");
            break;
        default:
            ESP_LOGI(TAG, "Other WebSocket event: %" PRId32, event_id);
            break;
    }
}

void app_websocket_start(void) {
    esp_websocket_client_config_t websocket_cfg = {
        .uri = WEBSOCKET_URL,
        .cert_pem = (const char *)gts_root_r1_pem_start,
    };

    esp_websocket_client_handle_t client =
        esp_websocket_client_init(&websocket_cfg);

    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to create WebSocket client");
        return;
    }

    ESP_ERROR_CHECK(esp_websocket_register_events(
        client,
        WEBSOCKET_EVENT_ANY,
        websocket_event_handler,
        NULL
    ));

    ESP_ERROR_CHECK(esp_websocket_client_start(client));

    ESP_LOGI(TAG, "WebSocket client started");
}