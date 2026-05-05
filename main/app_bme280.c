#include "app_bme280.h"

#include "bme280.h"
#include "i2c_bus.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "app_bme280";

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_SDA_GPIO 21
#define I2C_SCL_GPIO 20
#define I2C_MASTER_FREQ_HZ 100000
#define BME280_ADDR 0x77

void app_bme280_start(void)
{
    ESP_LOGI(TAG, "Initializing BME280");

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_GPIO,
        .sda_pullup_en = true,
        .scl_io_num = I2C_SCL_GPIO,
        .scl_pullup_en = true,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_bus_handle_t i2c_bus = i2c_bus_create(I2C_MASTER_NUM, &conf);
    if (i2c_bus == NULL) {
        ESP_LOGE(TAG, "Failed to create I2C bus");
        return;
    }

    bme280_handle_t bme280 = bme280_create(i2c_bus, BME280_ADDR);
    if (bme280 == NULL) {
        ESP_LOGE(TAG, "Failed to create BME280 handle");
        return;
    }

    esp_err_t init_result = bme280_default_init(bme280);
    ESP_LOGI(TAG, "init_result: %s", esp_err_to_name(init_result));

    vTaskDelay(pdMS_TO_TICKS(100));

    float temperature = -999.0f;
    float humidity = -999.0f;
    float pressure = -999.0f;

    esp_err_t temp_result = bme280_read_temperature(bme280, &temperature);
    esp_err_t hum_result = bme280_read_humidity(bme280, &humidity);
    esp_err_t press_result = bme280_read_pressure(bme280, &pressure);

    ESP_LOGI(TAG, "temp_result: %s", esp_err_to_name(temp_result));
    ESP_LOGI(TAG, "hum_result: %s", esp_err_to_name(hum_result));
    ESP_LOGI(TAG, "press_result: %s", esp_err_to_name(press_result));

    ESP_LOGI(TAG, "Temperature: %.2f C", temperature);
    ESP_LOGI(TAG, "Humidity: %.2f %%", humidity);
    ESP_LOGI(TAG, "Pressure: %.2f Pa", pressure);
}