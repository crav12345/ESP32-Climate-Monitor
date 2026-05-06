#pragma once

#include "esp_err.h"

esp_err_t app_websocket_send_text(const char *message);
void app_websocket_start(void);
