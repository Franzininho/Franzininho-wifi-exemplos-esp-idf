/*
 Autor : Equipe Embarcados
*/
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_YELLOW   GPIO_NUM_21
#define LED_GREEN    GPIO_NUM_33

TaskHandle_t task1_handler;
TaskHandle_t task2_handler;

void task_1(void *args)
{
   uint8_t led_value = 0;
   const int delay_ms = 500;

   gpio_num_t led_yellow = LED_YELLOW;
   gpio_set_direction(led_yellow, GPIO_MODE_OUTPUT);

   while (1)
   {
       gpio_set_level(led_yellow, led_value);
       led_value = !led_value;
       vTaskDelay(delay_ms / portTICK_PERIOD_MS);// Aguarda alguns segundos
   }
    vTaskDelete(NULL);// deleta a task
}

void task_2(void *args)
{
   uint8_t led_value = 0;
   const int delay_ms = 100;

   gpio_num_t led_green = LED_GREEN;
   gpio_set_direction(led_green, GPIO_MODE_OUTPUT);

   while (1)
   {
       gpio_set_level(led_green, led_value);
       led_value = !led_value;
       vTaskDelay(delay_ms / portTICK_PERIOD_MS);// Aguarda alguns segundos
   }
    vTaskDelete(NULL);// deleta a task
}
void app_main()
{

   xTaskCreate(
       &task_1,   // função    
       "task 1",  // nome da task
       2048,      // tamanho da taks em bytes         
       NULL,      // pârametro
       3,         // prioridade da task      
      &task1_handler);     // handler da task     

     xTaskCreate(
       &task_2,   // função    
       "task 2",  // nome da task
       2048,      // tamanho da taks em bytes         
       NULL,      // pârametro
       3,         // prioridade da task      
      &task2_handler);     // handler da task 
}
