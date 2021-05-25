/*
 Descrição: Exemplo que mostra Hello World utilizando quatro modalidade de saída de dados por meio do
 terminal e interagindo com um LED externo para indicação de estado.
 
 Autor: Halysson Junior
 Data : 28/03/21
*/


// Arquivos de cabeçalho
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h" 
#include "driver/gpio.h"


// Definição de pino

#define LED 2

// Variavel Global
char status[2] = {'L','H'}; // Caracter para indicar estado do LED H- HIGH e L - LOW


void app_main(void) // Main
{
    
    gpio_pad_select_gpio(LED);// Mapeamento do pino LED
    gpio_set_direction(LED,GPIO_MODE_OUTPUT); // Definição 
    
    printf("Exemplo - Hello World \n");// Mostra informação no terminal
    
    
    //Variaveis Local
     bool i = 0 ; // Varia o estado lógico do LED

      for (;;) {// Loop
       
        i= i^1; // Operação lógica XOR para alternar o estado do LED
        gpio_set_level(LED,i); // Envia os níveis lógicos 1 ou 0
 
       printf("Hello World, i am Franzininho WiFi ----> LED status : %c \n", status[i]);
        
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Rotina de Delay de 1 segundo
        fflush(stdout);  // Descarrega os buffers de saída de dados   
    
    }// end Loop
    
    
}// end Main
