idf_component_register(
    SRCS "app_time_sync.c" "main.c" "app_wifi.c" "app_websocket.c"
    INCLUDE_DIRS "."
    REQUIRES esp_websocket_client nvs_flash esp_wifi
)