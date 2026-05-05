#include "app_i2c_scan.h"

#include "driver/i2c_master.h"
#include "esp_log.h"

static const char *TAG = "app_i2c_scan";

#define I2C_SDA_GPIO 21
#define I2C_SCL_GPIO 20

void app_i2c_scan(void)
{
    ESP_LOGI(TAG, "Initializing I2C bus");

    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .sda_io_num = I2C_SDA_GPIO,
        .scl_io_num = I2C_SCL_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    i2c_master_dev_handle_t bme280_handle;

    i2c_device_config_t bme280_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x77,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(
        bus_handle,
        &bme280_config,
        &bme280_handle
    ));

    ESP_LOGI(TAG, "I2C bus initialized");

    ESP_LOGI(TAG, "Scanning I2C bus...");

    for (uint8_t address = 1; address < 127; address++) {
        esp_err_t result = i2c_master_probe(bus_handle, address, 100);

        if (result == ESP_OK) {
            ESP_LOGI(TAG, "Found device at 0x%02X", address);
        }
    }

    ESP_LOGI(TAG, "Scan complete");

    uint8_t chip_id = 0;
    uint8_t reg = 0xD0;

    ESP_ERROR_CHECK(i2c_master_transmit_receive(
        bme280_handle,
        &reg,
        1,
        &chip_id,
        1,
        100
    ));

    ESP_LOGI(TAG, "Chip ID: 0x%02X", chip_id);
}