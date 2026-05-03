#include "app_websocket.h"
#include "esp_websocket_client.h"
#include "esp_log.h"

static const char *TAG = "app_websocket";

#define WEBSOCKET_URL "wss://climate-monitor-server-397967553683.us-east1.run.app/"

extern const uint8_t gts_root_r1_pem_start[] asm("_binary_gts_root_r1_pem_start");
extern const uint8_t gts_root_r1_pem_end[] asm("_binary_gts_root_r1_pem_end");

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

    ESP_ERROR_CHECK(esp_websocket_client_start(client));

    ESP_LOGI(TAG, "WebSocket client started");
}