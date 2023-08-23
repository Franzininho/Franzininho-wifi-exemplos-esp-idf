/*
 Autor: Equipe Embarcados
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"

#define BUFFER_SIZE 256

//Declara o handler para o Message Buffer
MessageBufferHandle_t xmessagem_buffer;


void task_1 (void *pvParameters)
{
    char tx_data[] = "Hey Task 2 !!! ";
    size_t len = sizeof(tx_data);

    while(1)
    {
        xMessageBufferSend(xmessagem_buffer,tx_data,len,0);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void task_2 (void *pvParameters)
{
    char rx_data[50];
    int len = sizeof(rx_data);
    size_t recv_buffer;

    while(1)

    {
        recv_buffer = xMessageBufferReceive(xmessagem_buffer, (void *)rx_data, len, pdMS_TO_TICKS(10));
        
        if(recv_buffer > 0)
        {
            printf("[INFO] Recebi: %s \n",rx_data);
        }
    }
}

void app_main(void)
{
    //Cria o message buffer e alocar o espaço em memória (BUFFER SIZE)
    xmessagem_buffer = xMessageBufferCreate(BUFFER_SIZE);
    if(xmessagem_buffer == NULL)
    {
        printf("[ERRO] Falha ao criar o message buffer\n");
    }
    
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
}