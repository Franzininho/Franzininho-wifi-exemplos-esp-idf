#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "driver/rtc_io.h"

#define TAG "[DEEP MODE]"

static struct timeval sleep_enter_time;

#define GPIO_BTN 4
const int time_s = 10;

void deep_sleep_timer_wakeup(void)
{
    
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(time_s * 1000000));
}
void gpio_setup(void)
{
   gpio_config_t io_config =
   {
       .pull_up_en = GPIO_PULLUP_ENABLE,
       .pull_down_en = GPIO_PULLDOWN_DISABLE,
       .mode = GPIO_MODE_INPUT,
       .intr_type = GPIO_INTR_DISABLE,
       .pin_bit_mask = 1 << GPIO_BTN,
   };
   gpio_config(&io_config);
}

void app_main(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    int sleep_time = (now.tv_sec - sleep_enter_time.tv_sec) + (now.tv_usec - sleep_enter_time.tv_usec)/1000000;

    if(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)
    {
        ESP_LOGI(TAG, "Permaneceu ligado durante : %d segundos ", sleep_time - time_s);
    }
    gpio_setup();
    deep_sleep_timer_wakeup();

    while(1)
    {
        if (!gpio_get_level(GPIO_BTN))
        {
            vTaskDelay(pdMS_TO_TICKS(500));
            if (!gpio_get_level(GPIO_BTN))
            {
                    ESP_LOGI(TAG, "Indo mimi ..." );
                    esp_deep_sleep_start();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}