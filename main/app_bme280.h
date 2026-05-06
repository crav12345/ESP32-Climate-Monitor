#pragma once

#include "esp_err.h"

typedef struct {
    float temperature_c;
    float humidity_percent;
    float pressure_pa;
} app_bme280_reading_t;

esp_err_t app_bme280_init(void);
esp_err_t app_bme280_read(app_bme280_reading_t *reading);
void app_bme280_task(void *arg);