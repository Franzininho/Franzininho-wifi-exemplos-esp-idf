#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "relay.h"

// Define os pinos dos relés
#define RELAY1_PIN 21   //GPIO21
#define RELAY2_PIN 33   //GPIO33

void app_main() {

    // Cria as variáveis dos relés
    Relay relay1, relay2;   

    // Inicializa os relés
    relay_init(&relay1, RELAY1_PIN);    // Inicializa o relé 1
    relay_init(&relay2, RELAY2_PIN);    // Inicializa o relé 2

    while(1){                                                           // Loop infinito
        relay_turn_on(&relay1);                                         // Liga o relé 1
        printf("Status do relé 1: %d\n", relay_get_status(&relay1));    // Imprime o status do relé 1
        relay_turn_off(&relay2);                                        // Desliga o relé 2
        printf("Status do relé 2: %d\n", relay_get_status(&relay2));    // Imprime o status do relé 2
        vTaskDelay(5000 / portTICK_PERIOD_MS);                          // Aguarda 5 segundos
        relay_turn_off(&relay1);                                        // Desliga o relé 1
        printf("Status do relé 1: %d\n", relay_get_status(&relay1));    // Imprime o status do relé 1
        relay_turn_on(&relay2);                                         // Liga o relé 2
        printf("Status do relé 2: %d\n", relay_get_status(&relay2));    // Imprime o status do relé 2
        vTaskDelay(5000 / portTICK_PERIOD_MS);                          // Aguarda 5 segundos
    }
}