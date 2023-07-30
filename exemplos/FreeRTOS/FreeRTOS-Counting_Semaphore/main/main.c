#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define UART_NUM                UART_NUM_0
#define UART_RX_BUF_SIZE        (512)
#define UART_BAUD_RATE          115200

#define SEMAPHORE_ITEM 1

SemaphoreHandle_t xsemaphore_counting;

const size_t xbuffer_size = 1024;

static void uart_setup()
{
   uart_config_t uart_config = {
           .baud_rate = UART_BAUD_RATE,
           .data_bits = UART_DATA_8_BITS,
           .parity = UART_PARITY_DISABLE,
           .stop_bits = UART_STOP_BITS_1,
           .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
   };
   ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
   ESP_ERROR_CHECK(uart_set_pin(UART_NUM,
                   UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                   UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
   ESP_ERROR_CHECK(uart_driver_install(UART_NUM, UART_RX_BUF_SIZE, 0, 0, NULL, 0));
}

void task_1 (void *pvParameters)
{
    char data_task1[] = "{\"name\": \"Franzininho WiFi\", \"message\":\"Task 1 Semaphore Demo\"}";
    size_t len  = sizeof(data_task1);

    while (1)
    {
        xSemaphoreTake(xsemaphore_counting,portMAX_DELAY);
            uart_write_bytes(UART_NUM, &data_task1 , len);
            vTaskDelay(pdMS_TO_TICKS(500));
        xSemaphoreGive(xsemaphore_counting);
        
        vTaskDelay(1);
    }
}

void task_2 (void *pvParameters)
{
    char data_task2[] = "{\"name\": \"Franzininho WiFi\", \"message\":\"Task 2 Semaphore Demo\"}";
   size_t len  = sizeof(data_task2);

    while (1)
    {
        xSemaphoreTake(xsemaphore_counting,portMAX_DELAY);
            uart_write_bytes(UART_NUM, &data_task2 , len);
            vTaskDelay(pdMS_TO_TICKS(500));
        xSemaphoreGive(xsemaphore_counting);
        
        vTaskDelay(1);
    }
}

void app_main(void)
{
      xTaskCreate(
       &task_1,   // função    
       "task 1",  // nome da task
       2048,      // tamanho da taks em bytes         
       NULL,      // pârametro
       3,         // prioridade da task      
       NULL);     // handler da task     

     xTaskCreate(
       &task_2,   // função    
       "task 2",  // nome da task
       2048,      // tamanho da taks em bytes         
       NULL,      // pârametro
       3,         // prioridade da task      
       NULL);     // handler da task 
    
    //Configurar a UART
    uart_setup();
    
    //Cria um semaforo contador com no máximo 1 e inicia o valor em 1
    xsemaphore_counting = xSemaphoreCreateCounting(SEMAPHORE_ITEM,1);
    if(xsemaphore_counting == NULL)
    {
        printf("Erro ao criar o semafaro\n");
        return;
    }
    xSemaphoreGive(xsemaphore_counting);
}