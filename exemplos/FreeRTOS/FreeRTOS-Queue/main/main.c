/*
    Autor: Equipe Embarcados
*/
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define LED_YELLOW GPIO_NUM_21
#define BUTTON GPIO_NUM_2

#define QUEUE_LEN 5

QueueHandle_t xqueue_handler = NULL; // Handle da fila

uint64_t pulse_counter = 0;


static void IRAM_ATTR gpio_isr_handler(void* arg)
{
   pulse_counter++;
   xQueueSendFromISR(xqueue_handler,&pulse_counter, NULL);
}

static void gpio_configs (void)
{
   //Configurando o botão como entrada digital
   gpio_reset_pin(BUTTON);
   gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
   gpio_pullup_en(BUTTON);
  
   //Configurar a GPIO do botão para o ISR (Interrput Service Routine)
   gpio_install_isr_service(0);
   gpio_set_intr_type(BUTTON, GPIO_INTR_POSEDGE); 
   
   // GPIO_INTR_POSEDGE  - borda de subida
   // GPIO_INTR_NEGEDGE  - borda de descida
   // GPIO_INTR_ANYEDGE - qualquer tipo de board

   gpio_isr_handler_add(BUTTON, gpio_isr_handler, NULL);
  
   //Configurando o LED amarelo como saída digital
   gpio_reset_pin(LED_YELLOW);
   gpio_set_direction(LED_YELLOW, GPIO_MODE_OUTPUT);
}

void app_main()
{  
   uint8_t led_value = 0;
   uint64_t p_counter = 0;
   

   gpio_configs();
   printf("[Info]: GPIO configuradas\n");
   
  //Criação da fila
   xqueue_handler =  xQueueCreate(QUEUE_LEN, sizeof(uint64_t));
   if(xqueue_handler == NULL)
   {
       printf("[Erro]: Fila não foi criada\n");
       return;
   }
  
   while (1)
   {
      //Se receber algum na fila, então ..
      if (xQueueReceive(xqueue_handler, &p_counter, pdMS_TO_TICKS(1000)))
      {   
          //Alterna o estado do LED
           led_value = !led_value;
           gpio_set_level(LED_YELLOW, led_value);

           printf("Pulse: %lld\n", p_counter);
      }
   }
}
