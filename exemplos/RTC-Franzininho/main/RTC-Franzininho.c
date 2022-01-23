/*
   Autor: Halysson Junior
   Data : 10/04/21   
   Tutorial para acessar e configurar (horário ou data) do RTC DS3231
     
*/

#include <stdio.h>
#include <ds3231.h>

#define SDA_GPIO_ADS1115    18
#define SCL_GPIO_ADS1115    19


static i2c_dev_t RTC_DS3231;

struct tm franzininho_time = {
    .tm_year = 122, 
    .tm_mon  = 01,  
    .tm_mday = 30,
    .tm_hour = 14,
    .tm_min  = 00,
    .tm_sec  = 10
};

void app_main(void){

}
