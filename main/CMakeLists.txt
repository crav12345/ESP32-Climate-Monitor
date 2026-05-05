idf_component_register(
    SRCS "app_bme280.c" "app_i2c_scan.c" "app_led.c" "app_time_sync.c" "main.c" "app_wifi.c" "app_websocket.c"
    INCLUDE_DIRS "."
    EMBED_TXTFILES "certs/gts_root_r1.pem"
    REQUIRES esp_websocket_client nvs_flash esp_wifi espressif__cjson led_strip esp_driver_i2c
)