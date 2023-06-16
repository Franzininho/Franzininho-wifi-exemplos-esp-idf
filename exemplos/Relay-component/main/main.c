#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "relay.h"

//pin map
#define RELAY1_PIN 21
#define RELAY2_PIN 33

void app_main() {

    //instancia os componentes relés
    Relay relay1, relay2;

    // Inicializa os relés
    relay_init(&relay1, RELAY1_PIN);
    relay_init(&relay2, RELAY2_PIN);

    while(1){
        // Liga o relé 1
        relay_turn_on(&relay1);
        printf("Status do relé 1: %d\n", relay_get_status(&relay1));
        // Desliga o relé 2
        relay_turn_off(&relay2);
        printf("Status do relé 2: %d\n", relay_get_status(&relay2));
        // Aguarda 5 segundos
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        // Desliga o relé 1
        relay_turn_off(&relay1);
        printf("Status do relé 1: %d\n", relay_get_status(&relay1));
        // Liga o relé 2
        relay_turn_on(&relay2);
        printf("Status do relé 2: %d\n", relay_get_status(&relay2));
        // Aguarda 5 segundos
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}