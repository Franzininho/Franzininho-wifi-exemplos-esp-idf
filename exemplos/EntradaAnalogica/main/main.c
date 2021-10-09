/*
    Autor : Kayann Soares
    Utilização simples da Leitura de Dados Analogicos.
*/

// Bibliotecas Necessarias
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <driver/adc.h>

// Main
void app_main(){

  // Configurando a resolução do ADC para 13bits        
  adc1_config_width(ADC_WIDTH_BIT_13);
  // Configurando o Channel do ADC para o Channel 0
  adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11);


  while(1){

    // Função que de leitura do valor analogico, passando o ADC1 que é o do GPIO1
    int sensorValue = adc1_get_raw(ADC1_CHANNEL_0);

    // Imprimindo valores lidos
    printf("%d\n", sensorValue);

    // Rotina de Delay de 1 segundo
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    // Descarrega os buffers de saída de dados
    fflush(stdout);

  }
}
