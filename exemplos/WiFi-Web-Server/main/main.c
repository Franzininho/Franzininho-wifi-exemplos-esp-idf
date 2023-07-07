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
// #include "dns_server.h"

//Configurações do Acess Point

#define AP_SSID_HIDDEN 0
#define AP_AUTHMODE WIFI_AUTH_OPEN

#define AP_SSID "Franzininho AP"
#define AP_PASSWORD ""
#define AP_MAX_CONNECTION 10
#define AP_BEACON_INTERVAL 100 // Intervalo deve ser multipo de 100 (100 - 60000)

#define TAG_WEB_SERVER "[WEB SERVER]"


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
    //Configura par ao modo AP
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_config_t ap_cfg ={
       .ap = {
            .ssid = AP_SSID, 
            .password = AP_PASSWORD,
                .ssid_len =  0,
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
    
}