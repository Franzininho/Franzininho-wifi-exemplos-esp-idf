#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define LED_BLUE   GPIO_NUM_35
#define LED_GREEN  GPIO_NUM_36
#define LED_RED    GPIO_NUM_37

#define SW_GREEN_GPIO GPIO_NUM_2
#define SW_BLUE_GPIO  GPIO_NUM_7
#define SW_RED_GPIO   GPIO_NUM_14

#define LED_RED_BIT   BIT0
#define LED_BLUE_BIT  BIT1
#define LED_GREEN_BIT BIT2

EventGroupHandle_t xevent_group_handler = NULL;

static void gpio_configs (void)
{
 //Configurando o botão como entrada digital
 gpio_set_direction(SW_RED_GPIO, GPIO_MODE_INPUT);
 gpio_pullup_en(SW_RED_GPIO);

 gpio_set_direction(SW_BLUE_GPIO, GPIO_MODE_INPUT);
 gpio_pullup_en(SW_BLUE_GPIO);

 gpio_set_direction(SW_GREEN_GPIO, GPIO_MODE_INPUT);
 gpio_pullup_en(SW_GREEN_GPIO);

 //Configurando o LED azul como saída digital
 gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);
 gpio_set_direction(LED_GREEN, GPIO_MODE_OUTPUT);
 gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);

}

void vtask_led(void* pvParameters)
{
  
   const int time_out = 10;
   EventBits_t uxBits;
  
   while(1)
  
   {
       uxBits = xEventGroupWaitBits(
                                   xevent_group_handler,  //Event Group Handler
                                   LED_RED_BIT | LED_BLUE_BIT | LED_GREEN_BIT,// Bits
                                   pdFALSE,        
                                   pdFALSE,         
                                   pdMS_TO_TICKS(time_out)
                                   );
       // Caso a flag do LED vermelho seja pdTRUE ou 1,então aciona o LED
       if(uxBits & LED_RED_BIT)
       {
           gpio_set_level(LED_RED, 0);
       }
       // Caso contrário, desliga
       else
       {
           gpio_set_level(LED_RED, 1);
       }
      
       // Caso a flag do LED azul seja pdTRUE ou 1,então aciona o LED
       if ((uxBits & LED_BLUE_BIT))
       {
           gpio_set_level(LED_BLUE, 1);
       }
       // Caso contrário, desliga
       else
       {
           gpio_set_level(LED_BLUE, 0);
       }
      
       // Caso a flag do Verd azul seja pdTRUE ou 1,então aciona o LED
       if ((uxBits & LED_GREEN_BIT))
       {
           gpio_set_level(SW_BLUE_GPIO, 1);
       }
       // Caso contrário, desliga
       else
       {
           gpio_set_level(SW_BLUE_GPIO, 0);
       }

   }
}

void vtask_sw(void* pvParameters)
{
   for(;;)
   {
     
       if(!gpio_get_level(SW_GREEN_GPIO))
       {
           xEventGroupSetBits(xevent_group_handler, LED_GREEN);
       }
       else
       {
           xEventGroupClearBits(xevent_group_handler, LED_GREEN);
       }
      
       if(!gpio_get_level(SW_BLUE_GPIO))
       {
           xEventGroupSetBits(xevent_group_handler, LED_BLUE);
       }
       else
       {
           xEventGroupClearBits(xevent_group_handler, LED_BLUE);
       }
      
       if(!gpio_get_level(SW_RED_GPIO))
       {
           xEventGroupSetBits(xevent_group_handler, LED_RED);
       }
       else
       {
           xEventGroupClearBits(xevent_group_handler, LED_RED);
       }

       vTaskDelay(pdMS_TO_TICKS(1000));
   }
}

void app_main()
{
  gpio_configs();
 printf("[Info]: GPIO configuradas\n");

 xevent_group_handler = xEventGroupCreate();
 if(xevent_group_handler == NULL)
 {
   printf("[Erro]: Falha ao criar o event group\n");
   return;
 }
  printf("[Info]: Criação das tasks\n");

   xTaskCreate(
           vtask_led, // Função
           " task LED",// Identificador da task
           2048,// Tamanho da pilha
           NULL, // Sem parâmetros
           3, //Prioridade
           NULL);// Sem handler

   xTaskCreate(
           vtask_sw,// Função
           "sw task ",// Identificador da task
           2048,// Tamanho da pilha
           NULL,// Sem parâmetros
           3,//Prioridade
           NULL);// Sem handler
 }
