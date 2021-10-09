/*
   Autor: Halysson Junior
   Data : 10/04/21   

    Nesse exemplo foi configurado o periferico LEDC para controlar a intensidade do LED.
     
*/

// Inclusão de arqivos de cabeçalho
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

// Definição para configuração do periférico
#define LEDC_GPIO         2
#define LEDC_FADE_TIME    500
#define LEDC_RESOLUTION   1024
#define LEDC_FREQ         5000


void app_main(void) // Main
{

  //  Estrutura de dados para receber  as váriaveis  de configuração de frequência (1Hz, 100 Hz e outros), modo (HIGH ou LOW) e selecionar o TIMER (0,1,2)  
  
  ledc_timer_config_t timer = {                   // Configuração do timer 

      .speed_mode      = LEDC_LOW_SPEED_MODE,     // Modo de Velocidade -> LOW
      .duty_resolution = LEDC_TIMER_10_BIT,       // Resolução do do ciclo de trabalho (2^10 = 1024 valores)
      .timer_num       = LEDC_TIMER_0,            // Utilizado o TIMER 0
      .freq_hz         = LEDC_FREQ,               // Frequência de operação do sinal PWM
      .clk_cfg         = LEDC_AUTO_CLK            // Seleção automatica da fonte geradora do clock (interna ou externa)
    
  };

  
  ledc_timer_config(&timer); // Envia o endereço  da estrutura timer para a função de configuração do canal PWM 
 
 /*
   NOTA:
   A frequencia e a resolução do ciclo de trabalho são independentes. Para uma elevada frequência do PWM, 
   terá uma baixa resolução do ciclo de trabalho disponivel e vice-versa  
 */

 //  Estrutura de dados para receber  as váriaveis  de configuração de frequencia (1Hz, 100 Hz e outros), modo (HIGH ou LOW), selecionar o TIMER (0,1,2) 
  
  ledc_channel_config_t channel_LEDC = {
      .gpio_num   =   LEDC_GPIO,             // Seleciona o pino para atuar o PWM
      .speed_mode =   LEDC_LOW_SPEED_MODE,   // Modo de Velocidade -> LOW
      .channel    =   LEDC_CHANNEL_0,           
      .timer_sel  =   LEDC_TIMER_0,
      .duty       =   0,
      .hpoint     =   0
    
    };
  
  ledc_channel_config(&channel_LEDC);
 
 
  ledc_fade_func_install(0); // Inicializa o serviço do Fade
  
  for(;;){ // Loop

   // Define e inicia a função de fade no periférico LEDC
    ledc_set_fade_time_and_start( channel_LEDC.speed_mode , channel_LEDC.channel , 0               , LEDC_FADE_TIME , LEDC_FADE_WAIT_DONE);
    ledc_set_fade_time_and_start( channel_LEDC.speed_mode , channel_LEDC.channel , LEDC_RESOLUTION , LEDC_FADE_TIME , LEDC_FADE_WAIT_DONE);
  
  
  }// endLoop

}//endMain

/*
 *** Paramentrização da função " ledc_set_fade_time_and_start(); " ***

ledc_set_fade_time_and_start(ledc_mode_tspeed_mode, ledc_channel_tchannel, uint32_t target_duty, uint32_t max_fade_time_ms, ledc_fade_mode_tfade_mode)

  ledc_mode_tspeed_mode     -> modo do timer utilizado
  ledc_channel_tchannel     -> indice do canal LEDC (0-7)
  uint32_t target_duty      -> (2 ** resolução_ciclo_de_trabalho) -  1
  uint32_t max_fade_time_ms -> tempo máximo (ms) para o fading  
  ledc_fade_mode_tfade_mode ->  
*/
