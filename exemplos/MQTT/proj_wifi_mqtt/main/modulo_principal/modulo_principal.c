/* Módulo: MQTT 
 * Autor: Pedro Bertoleti
*/

/* Includes */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "modulo_principal.h"
#include "../prio_tasks.h"
#include "../stacks_sizes.h"

/* Includes de outros módulos */
#include "../modulo_mqtt/modulo_mqtt.h"
#include "../modulo_leds/modulo_leds.h"

/* Definições - debug */
#define MOD_PRINCIPAL_TAG "MOD_PRINCIPAL"

/* Defnições da tarefa deste módulo */
#define PARAMETROS_TASK_MODULO_PRINCIPAL  NULL
#define HANDLER_TASK_MODULO_PRINCIPAL     NULL

/* Definição - tempo entre mensagens enviadas por MQTT */
#define TEMPO_ENTRE_MSG_MQTT_MS           5000

/* Protótipo da tarefa deste módulo */
static void tarefa_modulo_principal(void *arg);

/* Função: inicializa modulo principal
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
void init_modulo_principal(void)
{
    ESP_LOGI(MOD_PRINCIPAL_TAG, "Inicializando modulo principal...");

    /* Agenda execução da tarefa do módulo principal */             
    xTaskCreate(tarefa_modulo_principal, "task_modulo_principal",
                            MODULO_PRINCIPAL_TAM_TASK_STACK,
                            PARAMETROS_TASK_MODULO_PRINCIPAL,
                            PRIO_TASK_MODULO_PRINCIPAL,
                            HANDLER_TASK_MODULO_PRINCIPAL);   

    ESP_LOGI(MOD_PRINCIPAL_TAG, "Modulo principal inicializado");
}

/* Função: tarefa do modulo principal
 * Parâmetros: argumentos passados a tarefa
 * Retorno: nenhum
 */
static void tarefa_modulo_principal(void *arg)
{
    char msg_mqtt_para_enviar_mqtt[MAX_STR_PARA_PUBLICAR] = {0};
    int estado_led_21 = 0;
    int estado_led_33 = 0;

    while(1)
    {
        if (verifica_se_mqtt_esta_conectado() == true)
        {
            /* Limpa o array que conterá a mensagem para enviar por MQTT */
            memset(msg_mqtt_para_enviar_mqtt, 0x00, MAX_STR_PARA_PUBLICAR);

            /* Le estado dos dois LEDs */            
            estado_led_21 = le_estado_led_franzininho_wifi(ID_LED_21);
            estado_led_33 = le_estado_led_franzininho_wifi(ID_LED_33);

            /* Formata a mensagem e envia por MQTT*/
            snprintf(msg_mqtt_para_enviar_mqtt, MAX_STR_PARA_PUBLICAR, "LED 21: %d  |  LED 33: %d", estado_led_21, estado_led_33);
            publica_mqtt(msg_mqtt_para_enviar_mqtt);     
        }

        vTaskDelay(TEMPO_ENTRE_MSG_MQTT_MS / portTICK_PERIOD_MS);
    }
}