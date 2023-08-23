// Inclusão de biblioteca
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/inet.h"

#include "esp_http_server.h"
#include "dns_server.h"

//Configurações do Acess Point

#define AP_SSID_HIDDEN 0
#define AP_AUTHMODE WIFI_AUTH_OPEN

#define AP_SSID "Franzininho AP"
#define AP_PASSWORD ""
#define AP_CHANNEL 1
#define AP_MAX_CONNECTION 10
#define AP_BEACON_INTERVAL 100 // Intervalo deve ser multipo de 100 (100 - 60000)

#define TAG_WEB_SERVER "[WEB SERVER]"


// Dados binários do aquivo HTML
extern const char index_start[] asm("_binary_index_html_start");
extern const char index_end[] asm("_binary_index_html_end");


static esp_err_t root_get_handler(httpd_req_t *req)
{
    const uint32_t root_len = index_end - index_start;

    ESP_LOGI(TAG_WEB_SERVER, "Serve root");
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_send(req, index_start, root_len);

    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler
};


esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    // Define o status 
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redireciona para o arqivo root (raiz)
    httpd_resp_set_hdr(req, "Location", "/");
 
    httpd_resp_send(req, "Redirecionando para o Captive Portal", HTTPD_RESP_USE_STRLEN);

    ESP_LOGI(TAG_WEB_SERVER, "Redirecionando para root");
    return ESP_OK;
}

static httpd_handle_t init_web_server(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = 3;
    config.lru_purge_enable = true;

    // Instância o servidor 
    ESP_LOGI(TAG_WEB_SERVER, "Web Server porta : '%d'", config.server_port); // Por padrão está na 80
    if (httpd_start(&server, &config) == ESP_OK) {
        // Configura os handles para a pagina inicial e caso a página não seja encontrada
        httpd_register_uri_handler(server, &root);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    return server;
}
// Função de callback para tratar os eventos do driver do WiFi 
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if(event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        // Reliza um cast com o event_data
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG_WEB_SERVER,"Dispositivo conectado! MAC ADDR :" 
                        "%02x:%02x:%02x:%02x:%02x:%02x",
                        event->mac[0],event->mac[1],event->mac[2],
                        event->mac[3],event->mac[4],event->mac[5]);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG_WEB_SERVER,"Dispositivo disconectado! MAC ADDR :" 
                        "%02x:%02x:%02x:%02x:%02x:%02x",
                        event->mac[0],event->mac[1],event->mac[2],
                        event->mac[3],event->mac[4],event->mac[5]);
    }
}

void app_main(void)
{
    //Inicializando o NVS
    esp_err_t ret = nvs_flash_init();
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    //Inicializa a stack TCP/IP
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    //Recebe as configurações padrões do driver WiFi 
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    
    //Alocar as configurações do driver do WiFi
    esp_wifi_init(&cfg);

     //Registra a função de callback para qualquer evento que ocorre no driver do WiFi
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    //Configura o modo de armazenamento para RAM
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    //Configura para o modo AP
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_config_t ap_cfg ={
       .ap = {
            .ssid = AP_SSID, 
            .password = AP_PASSWORD,
                .ssid_len =  strlen(AP_SSID),
                .channel = 1,
                .authmode = AP_AUTHMODE,
                .ssid_hidden = AP_SSID_HIDDEN,
                .max_connection = AP_MAX_CONNECTION,
                .beacon_interval = AP_BEACON_INTERVAL,
        },
    };
    
    // Passa as configurações do AP para o driver do WiFi
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&ap_cfg));    
    
    //Inicia o driver do WiFi
    ESP_ERROR_CHECK(esp_wifi_start());
     //Registra a função de callback para qualquer evento que ocorre no driver do WiFi
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    //Configura o modo de armazenamento para RAM
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    //Configura par ao modo AP
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP,&ap_cfg));    
    
    //Inicia o driver do WiFi
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);
    
    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    ESP_LOGI(TAG_WEB_SERVER, "Inicializando Web Server - IP: %s", ip_addr);
    
    init_web_server();
    init_dns_server();
}