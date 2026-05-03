#include "app_time_sync.h"
#include "esp_sntp.h"
#include "esp_log.h"
#include <time.h>

static const char *TAG = "app_time";

void app_time_sync(void)
{
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();

    time_t now = 0;
    struct tm timeinfo = {0};

    int retry = 0;
    while (timeinfo.tm_year < (2024 - 1900) && ++retry < 10) {
        ESP_LOGI(TAG, "Waiting for system time sync...");
        vTaskDelay(pdMS_TO_TICKS(2000));
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    ESP_LOGI(TAG, "Current time: %s", asctime(&timeinfo));
}