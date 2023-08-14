#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "esp_log.h"

#define GPIO_BTN 4 
#define TAG "[LIGHT MODE]"

void gpio_setup()
{
    gpio_config_t io_config =
    {
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = 1 << GPIO_BTN,
        .mode = GPIO_MODE_INPUT,
    };
    gpio_config(&io_config);

    gpio_wakeup_enable(GPIO_BTN, GPIO_INTR_LOW_LEVEL);
    ESP_LOGI (TAG, "Indo dormir... Pressione o botão para acorda");
    
    vTaskDelay(pdMS_TO_TICKS(1000));

    esp_sleep_enable_gpio_wakeup(); 
}

void app_main(void)
{
    int counter = 0;
    gpio_setup();
    
    while (gpio_get_level(GPIO_BTN))
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    ESP_LOGI (TAG, "Acordei !!!");
    
    for(;;)
    {
        ESP_LOGI(TAG, "Programa em execução %d", ++counter);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}