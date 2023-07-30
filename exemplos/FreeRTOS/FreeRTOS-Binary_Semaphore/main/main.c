#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define LED_BLUE    GPIO_NUM_21
#define BUTTON_GPIO GPIO_NUM_2
#define DEBOUNCE_MS 100

volatile TickType_t btn_deounce;
const int time_out = 2000;

SemaphoreHandle_t xsemaphore_handler = NULL;

static void gpio_configs (void)
{
 //Configurando o botão como entrada digital
 gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
 gpio_pullup_en(BUTTON_GPIO);

 //Configurando o LED azul como saída digital
 gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);

}

void vtask_flash_led(void* pvParameters)
{
   uint8_t led_status = 0;

   while(1)
   {
       if (xSemaphoreTake( xsemaphore_handler,pdMS_TO_TICKS(time_out) ) == pdTRUE )
       {
           if ((xTaskGetTickCount() - btn_deounce) < DEBOUNCE_MS)
           {
               led_status = !led_status;
               gpio_set_level(LED_BLUE, led_status);
               vTaskDelay(1000);
           }
       }
   }
}

void vtask_button(void* pvParameters)
{
   for(;;)
   {
       //Se o botão for pressionado, então ...
       if(!gpio_get_level(BUTTON_GPIO))
       {
           //Armazeno os ticks
           btn_deounce = xTaskGetTickCount();
           xSemaphoreGive(xsemaphore_handler);
       }
       vTaskDelay(pdMS_TO_TICKS(10));
   }
}


void app_main()
{
  gpio_configs();
  printf("[Info]: GPIO configuradas\n");

 xsemaphore_handler =  xSemaphoreCreateBinary();
 if (xsemaphore_handler == NULL)
 {
   printf("[Erro]: Semaforo não foi criada\n");
   return;
 }
  printf("[Info]: Criação das tasks\n");

   xTaskCreate(
           vtask_flash_led, // Função
           "flash LED",// Identificador da task
           4098,// Tamanho da pilha
           NULL, // Sem parâmetros
           3, //Prioridade
           NULL);// Sem handler

   xTaskCreate(
           vtask_button,// Função
           "button ",// Identificador da task
           2048,// Tamanho da pilha
           NULL,// Sem parâmetros
           3,//Prioridade
           NULL);// Sem handler
 }
