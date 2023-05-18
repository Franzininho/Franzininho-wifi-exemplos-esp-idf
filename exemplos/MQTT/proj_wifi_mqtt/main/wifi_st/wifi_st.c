/* Módulo: wi-fi (station)
 * Autor: Pedro Bertoleti
*/

/* Includes */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "wifi_st.h"

/* Includes de outros módulos */
#include "../modulo_mqtt/modulo_mqtt.h"

/* Definições - debug */
#define WIFI_TAG                "WIFI"

/* Variáveis esticas */
static bool wifi_conectado = false;

/* Funções locais */
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static bool init_wifi_st(uint8_t * pt_ssid, uint8_t * pt_pass);

/* Função: informa o status de conexao wifi
 * Parâmetros: nenhum
 * Retorno: true: wi-fi conectado
 *          false: wi-fi desconectado
*/
bool get_status_conexao_wifi(void)
{
    return wifi_conectado;
}

/* Função: callback de eventos do wifi (station)
 * Parâmetros: evento (base e id), argumentos e dados do evento
 * Retorno: nenhum
*/
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        /* Evento que indica que o wi-fi iniciou corretamente */
        ESP_LOGI(WIFI_TAG, "Conectando-se a rede wi-fi...");
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        /* Evento que indica que o wi-fi desconectou.
           A reconexão é feita automaticamente.
        */
        wifi_conectado = false;

        ESP_LOGI(WIFI_TAG, "Wi-fi foi desconetado. Reconectando-se a rede wi-fi...");
        esp_wifi_connect();
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        /* Evento que indica que o wi-fi se conectou e obteve IP na rede */
        wifi_conectado = true;

        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "Conexao wi-fi realizada. IP obtido:" IPSTR, IP2STR(&event->ip_info.ip));

        /* Inicializa MQTT e conecta-se ao broker */
        init_mqtt();
    }
    else if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(WIFI_TAG, "station "MACSTR" conectado, AID=%d", MAC2STR(event->mac), event->aid);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) 
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(WIFI_TAG, "station "MACSTR" desconectado, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

/* Função: inicializa wifi (station)
 * Parâmetros: nenhum
 * Retorno: nenhum
*/
void wifi_init_sta_st(void)
{
    uint8_t ssid_rede_wifi[TAM_MAX_SSID_ST_WIFI] = {0};
    uint8_t pass_rede_wifi[TAM_MAX_PASS_ST_WIFI] = {0};
    uint8_t ip_gateway[TAM_MAX_IP_GATEWAY] = {0};
    uint8_t mascara_gateway[TAM_MAX_GATEWAY_MASK] = {0};
    bool inicializacao_wifi_st = true;
  
    /* Configura credenciais wifi, gateway e mascara de rede */
    snprintf((char *)ssid_rede_wifi, sizeof(ssid_rede_wifi), "%s", WIFI_SSID_ST_DEFAULT);
    ESP_LOGI(WIFI_TAG, "SSID wifi: %s\n", ssid_rede_wifi);
    
    snprintf((char *)pass_rede_wifi, sizeof(pass_rede_wifi), "%s", WIFI_PASS_ST_DEFAULT);
    ESP_LOGI(WIFI_TAG, "Senha wifi: %s\n", pass_rede_wifi);
    
    /* IP do gateway */
    snprintf((char *)ip_gateway, sizeof(ip_gateway), "%s", WIFI_ST_GATEWAY);
    ESP_LOGI(WIFI_TAG, "IP do gateway: %s\n", ip_gateway);
    
    /* Mascara do gateway */
    snprintf((char *)mascara_gateway, sizeof(mascara_gateway), "%s", WIFI_ST_NETWORK_MASK);
    ESP_LOGI(WIFI_TAG, "Mascara de rede: %s\n", mascara_gateway);

    /* Inicializa wifi */
    wifi_conectado = false;
    ESP_LOGI(WIFI_TAG, "Inicializando wi-fi (station)...\n");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Inicializa wifi station */
    esp_netif_create_default_wifi_sta();    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Registra callbacks para eventos de wifi e eventos relacionados a conexao (ip na rede) */
    ESP_LOGI(WIFI_TAG, "Registrando callbacks de eventos wifi e eventos de conexao de rede...\n");
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    ESP_LOGI(WIFI_TAG, "Callbacks de eventos wifi e eventos de conexao de rede registrados\n");
 
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
    ESP_ERROR_CHECK( esp_wifi_start() );

    /* Configura SSID, senha e segurança do wi-fi */ 
    /* Station */
    inicializacao_wifi_st = init_wifi_st(ssid_rede_wifi, pass_rede_wifi);

    if (inicializacao_wifi_st == true)
    {
        ESP_LOGI(WIFI_TAG, "Wi-fi OK (station)\n");   
    }
    else
    {
        ESP_LOGE(WIFI_TAG, "Falha ao inicializar Wi-fi (station)\n");   
    }
}

/* Função: inicializa wifi (st)
 * Parâmetros: nenhum
 * Retorno: true: inicialiação ok
 *          false: falha ao inicializar
*/
static bool init_wifi_st(uint8_t * pt_ssid, uint8_t * pt_pass)
{
    bool status_wifi_st = false;
    wifi_config_t wifi_config = {0};

    snprintf((char *)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", (char *)pt_ssid);
    snprintf((char *)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", (char *)pt_pass);
    
    esp_err_t ret_esp_set_mode = esp_wifi_set_mode(WIFI_MODE_STA);
    esp_err_t ret_esp_wifi_set_config = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_connect();
    
    if ( (ret_esp_set_mode == ESP_OK) && (ret_esp_wifi_set_config == ESP_OK) )
    {
        status_wifi_st = true;
    }
    else
    {
        status_wifi_st = false;
    }
      
    if (status_wifi_st == true)
    {
        ESP_LOGI(WIFI_TAG, "Wi-fi ST inicializado OK\n");
    }
    else
    {
        ESP_LOGE(WIFI_TAG, "Falha ao inicializar Wi-fi ST\n");
    }

    return status_wifi_st;
}