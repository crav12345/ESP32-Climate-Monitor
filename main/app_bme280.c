#include "app_bme280.h"
#include "app_websocket.h"

#include "bme280.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_bus.h"
#include <stdio.h>

static const char *TAG = "app_bme280";

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_SDA_GPIO 21
#define I2C_SCL_GPIO 20
#define I2C_MASTER_FREQ_HZ 100000
#define BME280_ADDR 0x77

static i2c_bus_handle_t s_i2c_bus = NULL;
static bme280_handle_t s_bme280 = NULL;

esp_err_t app_bme280_init(void) {
  ESP_LOGI(TAG, "Initializing BME280");

  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_SDA_GPIO,
      .sda_pullup_en = true,
      .scl_io_num = I2C_SCL_GPIO,
      .scl_pullup_en = true,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };

  s_i2c_bus = i2c_bus_create(I2C_MASTER_NUM, &conf);
  if (s_i2c_bus == NULL) {
    ESP_LOGE(TAG, "Failed to create I2C bus");
    return ESP_FAIL;
  }

  s_bme280 = bme280_create(s_i2c_bus, BME280_ADDR);
  if (s_bme280 == NULL) {
    ESP_LOGE(TAG, "Failed to create BME280 handle");
    return ESP_FAIL;
  }

  esp_err_t result = bme280_default_init(s_bme280);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize BME280: %s", esp_err_to_name(result));
    return result;
  }

  ESP_LOGI(TAG, "BME280 initialized");
  return ESP_OK;
}

esp_err_t app_bme280_read(app_bme280_reading_t *reading) {
  if (reading == NULL) {
    return ESP_ERR_INVALID_ARG;
  }

  if (s_bme280 == NULL) {
    return ESP_ERR_INVALID_STATE;
  }

  esp_err_t result;

  result = bme280_read_temperature(s_bme280, &reading->temperature_c);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read temperature: %s", esp_err_to_name(result));
    return result;
  }

  result = bme280_read_humidity(s_bme280, &reading->humidity_percent);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read humidity: %s", esp_err_to_name(result));
    return result;
  }

  result = bme280_read_pressure(s_bme280, &reading->pressure_pa);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read pressure: %s", esp_err_to_name(result));
    return result;
  }

  ESP_LOGI(TAG, "BME280: %.2f C, %.2f %%, %.2f Pa", reading->temperature_c,
           reading->humidity_percent, reading->pressure_pa);

  return ESP_OK;
}

void app_bme280_task(void *arg) {
  (void)arg;

  while (true) {
    app_bme280_reading_t reading;

    esp_err_t result = app_bme280_read(&reading);
    if (result != ESP_OK) {
      ESP_LOGE(TAG, "BME280 read failed: %s", esp_err_to_name(result));
    } else {
      char message[256];

      snprintf(message, sizeof(message),
               "{\"type\":\"telemetry\",\"temperatureC\":%.2f,"
               "\"humidityPercent\":%.2f,\"pressurePa\":%.2f}",
               reading.temperature_c, reading.humidity_percent,
               reading.pressure_pa);

      app_websocket_send_text(message);
    }

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}