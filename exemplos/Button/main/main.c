/* 
    Autor : Halysson Junior
    Data: 31/03/21
    Descrição:
    Neste exemplo utilizou-se um botão táctil (entrada digital) para acionar um LED (saída digital) 
    e mostra seu estado no terminal. 
    
*/
    
    

// Inclusão arquivos de cabeçalho

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"



// Definições de Pinos
#define BTN 15 
#define LED 2 

// Definição de habilitação (PULLUP / PULLDOWN)
#define PULLUP
// #define PULLDOWN (Para habilitar pulldown descomente aqui e comente -> " #define PULLUP ")

// Definições de estado lógico (caso altere para pulldown, deve-se inverter os estado lógicos)
#define ON 1
#define OFF 0

void app_main(void){  // Main


 
 gpio_pad_select_gpio(LED);                
 gpio_set_direction(LED,GPIO_MODE_OUTPUT);// onfigura LED como saída digital


 gpio_pad_select_gpio(BTN);  
 gpio_set_direction(BTN,GPIO_MODE_INPUT); // Configura LED como saída digital
 
  #ifdef PULLDOWN // Habilita pulldown 
  
   gpio_pulldown_en(BTN);
   gpio_pullup_dis(BTN);  
  
  #endif
  
  #ifdef PULLUP  // Habilita pullup
  gpio_pullup_en(BTN);
  gpio_pulldown_dis(BTN); 
  
  #endif

// Váriavel Local
bool last_state_btn = 0; // Armazena o último estado do botão       

    while (1) { // Loop
     
    // Váriavel Local  
     bool state_btn = gpio_get_level(BTN); // Leitura do botão
      
      if(!state_btn && !last_state_btn) {
          gpio_set_level(LED,ON);           // Se botão for zero então ... liga LED
          printf("LED LIGADO");             // Mostra informação no monitor "LED LIGADO"
          last_state_btn = true;            // Botão pressionado 
      }
      
      else if(state_btn && last_state_btn){
        gpio_set_level(LED,OFF);            // Senão... desliga LED
         printf ("LED DESLIGADO");          // Mostra informação no monitor "LED DESLIGADO"
        last_state_btn = false;             // Botão solto
      }
      vTaskDelay(1/ portTICK_PERIOD_MS);    // Rotina de Delay de 1 segundo
      fflush(stdout);                       // Descarrega os buffers de saída de dados   

    }// endLoop

}// endMain
