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
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_err.h"
#include "mqtt_client.h"
#include "modulo_mqtt.h"

/* Includes de outros módulos */
#include "../modulo_leds/modulo_leds.h"

/* Definições - debug */
#define MOD_MQTT_TAG "MOD_MQTT"

/* Variáveis estaticas */
static bool mqtt_conectado = false;
static esp_mqtt_client_handle_t client = NULL;

/* Função: handler de eventos do MQTT
 * Parâmetros: argumentos e dados do event ocorrido
 * Retorno: nenhum
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(MOD_MQTT_TAG, "Evento: MQTT_EVENT_CONNECTED (a Franzininho se conectou ao broker MQTT)");
        mqtt_conectado = true;

        /* Subscreve ao tópico de recepção de dados do servidor MQTT */
        msg_id = esp_mqtt_client_subscribe(client, MQTT_TOPICO_RECEPCAO, 0);
        ESP_LOGI(MOD_MQTT_TAG, "Franzininho wifi subscrita com sucesso no topico MQTT %s, msg_id=%d", MQTT_TOPICO_RECEPCAO, msg_id);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(MOD_MQTT_TAG, "Evento: MQTT_EVENT_DISCONNECTED (a Franzininho se desconectou do broker MQTT)");
        mqtt_conectado = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(MOD_MQTT_TAG, "Evento: MQTT_EVENT_SUBSCRIBED (a Franzininho fez um subscribe em um topico MQTT), msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(MOD_MQTT_TAG, "Evento: MQTT_EVENT_UNSUBSCRIBED (a Franzininho removeu o subscribe de um topico MQTT), msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(MOD_MQTT_TAG, "Evento: MQTT_EVENT_PUBLISHED (a Franzininho fez um publish via MQTT), msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(MOD_MQTT_TAG, "Evento: MQTT_EVENT_DATA. Foi recebido um payload via MQTT.");
        ESP_LOGI(MOD_MQTT_TAG, "---------");
        ESP_LOGI(MOD_MQTT_TAG, "Topico: %.*s\r", event->topic_len, event->topic);
        ESP_LOGI(MOD_MQTT_TAG, "Payload recebido: %.*s\r", event->data_len, event->data);
        ESP_LOGI(MOD_MQTT_TAG, "---------");

        /* Controla o LED 21 ou LED 33 */
        toogle_led_franzininho_wifi(event->data[0]);       

        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(MOD_MQTT_TAG, "Evento: MQTT_EVENT_ERROR");
        break;

    default:
        ESP_LOGI(MOD_MQTT_TAG, "Evento desconhecido. ID: %d", event->event_id);
        break;
    }
}

/* Função: inicializa MQTT
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
void init_mqtt(void)
{
    ESP_LOGI(MOD_MQTT_TAG, "Inicializando modulo MQTT...");

    /* Faz a conexão ao broker MQTT */
    mqtt_conectado = 0;
    esp_mqtt_client_config_t config_mqtt = {
         .uri = "mqtt://broker.hivemq.com:1883"
    };

    client = esp_mqtt_client_init(&config_mqtt);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

    ESP_LOGI(MOD_MQTT_TAG, "Modulo MQTT inicializado");
}

/* Função: publica mensagem MQTT
 * Parâmetros: ponteiro para string da mensagem a ser publicada
 * Retorno: nenhum
 */
int publica_mqtt(char *pt_str_para_publicar)
{
    char str_para_publicar[MAX_STR_PARA_PUBLICAR] = {0};
    int status_envio_mqtt = 0;

    /* Comporta a mensgem em um array com tamanho definido
       e publica a mensagem via MQTT
    */
    snprintf(str_para_publicar, MAX_STR_PARA_PUBLICAR, "%s", pt_str_para_publicar);
    status_envio_mqtt = esp_mqtt_client_publish(client, MQTT_TOPICO_ENVIO, str_para_publicar, CALCULA_TAM_MSG_PUBLICADA,
                                                QOS_UTILIZADO,
                                                FLAG_RETAIN);

    if (status_envio_mqtt == -1)
    {
        ESP_LOGE(MOD_MQTT_TAG, "Falha ao enviar mensagem MQTT");
    }
    else
    {
        ESP_LOGI(MOD_MQTT_TAG, "Mensagem enviada com sucesso via MQTT");
    }

    return status_envio_mqtt;
}

/* Função: retorna o status da coenxao MQTT
 * Parâmetros: nenhum
 * Retorno: true: Franzininho wifi está conectada ao broker MQTT
 *          false: Franzininho wifi não está conectada ao broker MQTT
 */
bool verifica_se_mqtt_esta_conectado(void)
{
    return mqtt_conectado;
}