#include "app_websocket.h"
#include "esp_websocket_client.h"
#include "esp_log.h"
#include <inttypes.h>
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
#include "app_led.h"

static const char *TAG = "app_websocket";
static esp_websocket_client_handle_t s_websocket_client = NULL;

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
            char *json_str = strndup((char *)data->data_ptr, data->data_len);

            if (json_str == NULL) {
                ESP_LOGE(TAG, "Failed to allocate JSON string");
                break;
            }

            cJSON *json = cJSON_Parse(json_str);

            if (json == NULL) {
                ESP_LOGE(TAG, "Failed to parse JSON");
                free(json_str);
                break;
            }

            cJSON *type = cJSON_GetObjectItem(json, "type");

            if (!cJSON_IsString(type)) {
                ESP_LOGW(TAG, "JSON message missing valid type");
                cJSON_Delete(json);
                free(json_str);
                break;
            }

            ESP_LOGI(TAG, "JSON message type: %s", type->valuestring);

            if (strcmp(type->valuestring, "setColor") == 0) {
                ESP_LOGI(TAG, "Handling setColor command");
                cJSON *color = cJSON_GetObjectItem(json, "color");

                if (cJSON_IsString(color)) {
                    const char *color_str = color->valuestring;

                    if (color_str[0] == '#' && strlen(color_str) == 7) {
                        unsigned int hex;

                        if (sscanf(color_str + 1, "%06x", &hex) == 1) {
                            uint8_t r = (hex >> 16) & 0xFF;
                            uint8_t g = (hex >> 8) & 0xFF;
                            uint8_t b = hex & 0xFF;

                            app_led_set_color(r, g, b);
                        } else {
                            ESP_LOGW(TAG, "Failed to parse hex color");
                        }
                    } else {
                        ESP_LOGW(TAG, "Invalid hex color format");
                    }
                } else {
                    ESP_LOGW(TAG, "setColor missing valid color string");
                }
            } else {
                ESP_LOGW(TAG, "Unknown message type: %s", type->valuestring);
            }

            cJSON_Delete(json);
            free(json_str);
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

    s_websocket_client = esp_websocket_client_init(&websocket_cfg);

    if (s_websocket_client == NULL) {
        ESP_LOGE(TAG, "Failed to create WebSocket client");
        return;
    }

    ESP_ERROR_CHECK(esp_websocket_register_events(
        s_websocket_client,
        WEBSOCKET_EVENT_ANY,
        websocket_event_handler,
        NULL
    ));

    ESP_ERROR_CHECK(esp_websocket_client_start(s_websocket_client));

    ESP_LOGI(TAG, "WebSocket client started");
}

esp_err_t app_websocket_send_text(const char *message)
{
    if (message == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_websocket_client == NULL) {
        ESP_LOGW(TAG, "WebSocket client not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (!esp_websocket_client_is_connected(s_websocket_client)) {
        ESP_LOGW(TAG, "WebSocket not connected");
        return ESP_ERR_INVALID_STATE;
    }

    int result = esp_websocket_client_send_text(
        s_websocket_client,
        message,
        strlen(message),
        1000
    );

    if (result < 0) {
        ESP_LOGE(TAG, "Failed to send WebSocket message");
        return ESP_FAIL;
    }

    return ESP_OK;
}