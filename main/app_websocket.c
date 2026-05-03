#include "app_websocket.h"
#include "esp_crt_bundle.h"
#include "esp_websocket_client.h"
#include "esp_log.h"

static const char *TAG = "app_websocket";

#define WEBSOCKET_URL "wss://climate-monitor-server-397967553683.us-east1.run.app/"

void app_websocket_start(void) {
    esp_websocket_client_config_t websocket_cfg = {
        .uri = WEBSOCKET_URL,
        .crt_bundle_attach = esp_crt_bundle_attach
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