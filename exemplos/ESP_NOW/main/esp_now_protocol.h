#pragma once

#include "esp_now.h"
#include "esp_log.h"
#include "string.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void esp_now_protocol_init();
void wifi_init();