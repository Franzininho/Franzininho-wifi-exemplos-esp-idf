/*
    Autor : Equipe Embarcados
*/
// Inclusão das bibliotecas 

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#define NAMESPACE "storage"

#define TAG "[NVS DEMO]"

#define GPIO_BTN 4

int64_t millis()
{
    return esp_timer_get_time()/1000;
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

void check_NVS ()
{
    nvs_handle_t nvs_handle;
    esp_err_t ret;
    int64_t time = 0;

    ret = nvs_open(NAMESPACE,NVS_READWRITE, &nvs_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Erro ao abrir o NVS");
        return;
    }

    ret = nvs_get_i64(nvs_handle,"boot_time",&time);
    
    if(ret != ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGW(TAG, "Chave não encontrada no NVS");
    }
    else 
    {
        ret = nvs_set_i64(nvs_handle, "boot_time",time);
        nvs_commit(nvs_handle);
    }
    
    nvs_close(nvs_handle);
    
    ESP_LOGI(TAG, "Tempo total ligado da última vez: %" PRIu64 " s", time/1000);
}

void save_NVS (int64_t last_time)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret;

    ret = nvs_open(NAMESPACE, NVS_READWRITE, &nvs_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Erro ao abrir o NVS");
        return;
    }
    
    ret = nvs_set_i64(nvs_handle, "boot_time",last_time);
    if(ret != ESP_OK)
    {
        ESP_LOGE (TAG, "Erro ao escrever o valor");
    }

    nvs_commit(nvs_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE (TAG, "Erro ao salvar");
    }

    nvs_close(nvs_handle);

    ESP_LOGI (TAG, "Salvo");
}

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // Se na partição NVS ocorreu erro, precisa-se limpar o espaço em memória
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK( err );
    
    check_NVS();

    gpio_setup();

    uint64_t current_time = 0;

    while(1)
    {
        if(!gpio_get_level(GPIO_BTN))
        {
            vTaskDelay(pdMS_TO_TICKS(500));
            if(!gpio_get_level(GPIO_BTN))
            {
                ESP_LOGI(TAG, "Botão pressionado ...");
                current_time = millis();
                ESP_LOGI(TAG, "Time: %lld ", current_time/1000);
                save_NVS (current_time);
                esp_restart();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));

    }
}