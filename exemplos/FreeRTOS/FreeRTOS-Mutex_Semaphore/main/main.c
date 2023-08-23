/*
    Autor: Equipe Embarcados
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"

#define LED_YELLOW   GPIO_NUM_21
#define LED_GREEN    GPIO_NUM_33 

SemaphoreHandle_t xMutex;

static void gpio_setup()
{
   gpio_reset_pin(LED_GREEN);
   gpio_reset_pin(LED_YELLOW);

   gpio_set_direction(LED_GREEN, GPIO_MODE_OUTPUT);
   gpio_set_direction(LED_YELLOW, GPIO_MODE_OUTPUT);

   gpio_set_level(LED_GREEN, 0);
   gpio_set_level(LED_YELLOW, 0);
}

void task_2( void *pvParameters )
{
  int i;
  const int counter = 10;

  while(1)
  {   
    xSemaphoreTake( xMutex, portMAX_DELAY );
    
    printf("Executando Task 2 ... \n");
    gpio_set_level(LED_GREEN, 1);

    for( i = 0; i<counter; i++)
    {
        printf("Counter: %d \n", i);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    printf("Task 2 Finalizada \n");
    gpio_set_level(LED_GREEN, 0);
    xSemaphoreGive( xMutex );
  }
}

void task_1( void *pvParameters )
{

  while(1)
  {
    xSemaphoreTake( xMutex, portMAX_DELAY );
    
    printf("Executando Task 1 ... \n");
    
    gpio_set_level(LED_YELLOW, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(LED_GREEN, 0);
    printf("Task 1 Finalizada \n");

    xSemaphoreGive( xMutex );
    
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void app_main (void)
{   
  
  gpio_setup();

  xMutex = xSemaphoreCreateMutex();
    
    xTaskCreate(
    &task_1,   // função    
    "task 1",  // nome da task
    2048,      // tamanho da taks em bytes         
    NULL,      // pârametro
    5,         // prioridade da task      
    NULL);     // handler da task     

    xTaskCreate(
    &task_2,   // função    
    "task 2",  // nome da task
    2048,      // tamanho da taks em bytes         
    NULL,      // pârametro
    5,         // prioridade da task      
    NULL);     // handler da task
}