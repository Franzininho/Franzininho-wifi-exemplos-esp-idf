/*
    Autor : Equipe Embarcados / Comunidade Franzininho
    Descrição: 
        Firmware de exmplo para envio de mensagem via MQTT para o TagoIO

*/

// Inclusão de bibliotecas
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/temp_sensor.h"

#define WIFI_SSID "seu ssid"
#define WIFI_PASSWORD "sua senha"
#define MQTT_TOKEN "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxx"
#define MQTT_ID_CLIENT "Franzininho_WiFi_MQTT"

#define TAG  "MQTT"

// Declarando o handler do client global
static esp_mqtt_client_handle_t client = NULL;

static esp_err_t mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    
    switch (event_id) 
    {
        case MQTT_EVENT_CONNECTED:
            
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "tago/data/post", 0);
            ESP_LOGI(TAG, "Envio do subscribe com sucesso, msg_id=%d", msg_id);
            
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "publish enviado com sucesso, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}


static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    switch(event_id) 
    { 
        case WIFI_EVENT_STA_START:
           
            ESP_LOGI(TAG,"Conectando ao roteador");
            break;    
        
        case WIFI_EVENT_STA_DISCONNECTED:

            ESP_LOGE(TAG,"Falha ao conectar ao Ponto de Acesso");
            break;
        
     
        case IP_EVENT_STA_GOT_IP: 
        
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG, "IP:" IPSTR, IP2STR(&event->ip_info.ip));
            break;
    }
}
//Configurando o driver do WiFi
static void wifi_init(void)
{
   
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
   
    
    esp_event_handler_instance_t instance_any_id; //Handler para indentificar o contexto do evento 
    esp_event_handler_instance_t instance_got_ip;//Handler para indentificar o contexto do evento 
    
    //Registra as instâncias 
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));
    wifi_config_t wifi_config = 
    {
        .sta = 
        {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); // Configura o driver para moodo Station
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config)); // Passa a configuração de SSID e senha 

    ESP_LOGI(TAG, "SSID:[%s] Senha:[%s]", WIFI_SSID, WIFI_SSID);
    
    ESP_ERROR_CHECK(esp_wifi_start());//Inicia o wifi
    ESP_ERROR_CHECK(esp_wifi_connect());//Perfoma a conexão com a rede
    ESP_LOGI(TAG, "Aguardando conectar ...");

   
}

static void mqtt_app_start(void)
{
    
    ESP_LOGI(TAG, "Inicializando modulo MQTT...");

    /* Realiza a conexão ao broker MQTT */

    esp_mqtt_client_config_t config_mqtt = 
    {
        .broker.address.uri = "mqtt://mqtt.tago.io:1883",// URI do Tago IO
        .credentials.authentication.password = MQTT_TOKEN, // Token do device
        .credentials.username = "tago", // Nome de usário
        .credentials.client_id = MQTT_ID_CLIENT // Nome do seu device
    };

    //Cria um handler para o client
    client = esp_mqtt_client_init(&config_mqtt);
    //Registra a função de callback para o handler do client
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    
    //Inicializa o client, autenticação no broken
    if(esp_mqtt_client_start(client)!=ESP_OK)
    {
        ESP_LOGW(TAG, "MQTT não inicializado. Confira a URI ou as credênciais");
        return;
    }

    ESP_LOGI(TAG, "MQTT inicializado");
}

void app_main()
{
   
    nvs_flash_init(); // Inicializando o NVS
    wifi_init(); // Inicializando WiFi
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();  
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT;                
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();

    vTaskDelay(pdMS_TO_TICKS(10000)); //Aguarda 10s até estabilizar a conexão com a rede
    mqtt_app_start(); //Cria o novo client e conecta-se ao broken
    
    while(1) // Loop
    {
        char msg[50];
        float temp_out;                                            //temperature sensor output variable
        temp_sensor_read_celsius(&temp_out);                      // ler a temperatura do sensor
        sprintf(msg, "[{\"variable\": \"temperature\",\"value\": %.2f}]",temp_out); // Cria uma mensagem fake para publicar no broken
        esp_mqtt_client_publish(client, "tago/data/post", msg, 0, 1, 0);// Client envia o publish para o Broken
        vTaskDelay(pdMS_TO_TICKS(10000)); //Aguarda 10 segundo para o próximo envio.
    }
}