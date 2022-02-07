/*
   Autor: Halysson Junior
   Data : 10/04/21   
   Tutorial para acessar e configurar (horário ou data) do RTC DS3231
     
*/

#include <stdio.h>
#include <esp_log.h>
#include <ds3231.h>
#include <string.h>

#define SDA_GPIO_RTC    16
#define SCL_GPIO_RTC    17


static i2c_dev_t RTC_DS3231;

struct tm franzininho_time = {
    .tm_year = 122, 
    .tm_mon  = 01,  
    .tm_mday = 30,
    .tm_hour = 14,
    .tm_min  = 00,
    .tm_sec  = 10
};

void config_time_ds3231(uint16_t year, uint16_t mon, uint16_t mday, uint16_t hour, uint16_t min, uint16_t sec ){
    esp_err_t res;

    /*Atualiza valores de hora e data */
    franzininho_time.tm_year  = year;
    franzininho_time.tm_mon   = mon;
    franzininho_time.tm_mday  = mday;
    franzininho_time.tm_hour  = hour;
    franzininho_time.tm_min   = min;
    franzininho_time.tm_sec   = sec;

    
    memset(&RTC_DS3231, 0, sizeof(i2c_dev_t));
    res = ds3231_init_desc(&RTC_DS3231, 0, SDA_GPIO_RTC, SCL_GPIO_RTC);
    if(res != ESP_OK) ESP_LOGW("AVISO", "RTC não inicializado");
    
    res = ds3231_set_time(&RTC_DS3231, &franzininho_time);
    if(res != ESP_OK) ESP_LOGW("AVISO", "RTC não configurado");
}

esp_err_t get_current_time(){
     if (ds3231_get_time(&RTC_DS3231, &franzininho_time) != ESP_OK){
         ESP_LOGW("AVISO", "Erro na leitura do RTC");
         return ESP_FAIL;   
    }else{
        ESP_LOGI("FRANZININHO CLOCK","DATA - %02d/%02d/%04d HORA - %02d:%02d:%02d", 
                                                                franzininho_time.tm_mday,    
                                                                franzininho_time.tm_mon + 1,
                                                                franzininho_time.tm_year + 1900, 

                                                                franzininho_time.tm_hour, 
                                                                franzininho_time.tm_min, 
                                                                franzininho_time.tm_sec);
    }
    return ESP_OK;
}

void init_RTC(void){
    esp_err_t res ;
    memset(&RTC_DS3231,0,sizeof(i2c_dev_t));
    res = ds3231_init_desc(&RTC_DS3231, 0, SDA_GPIO_RTC, SCL_GPIO_RTC);
    if(res != ESP_OK) ESP_LOGW("AVISO", "RTC não inicializado");
}



void app_main(void){

    ESP_LOGI("FRANZININHO CLOCK", "Inicializando o exemplo teste");
    init_RTC();

    while(1){

        get_current_time();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}
