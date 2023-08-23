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


// Configurações do Acess Point

#define AP_SSID_HIDDEN 0
#define AP_AUTHMODE WIFI_AUTH_OPEN

#define AP_SSID "Franzininho AP"
#define AP_PASSWORD ""
#define AP_MAX_CONNECTION 10
#define AP_BEACON_INTERVAL 100 // Intervalo deve ser multipo de 100 (100 - 60000)

#define TAG_AP "[AP]"

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if(event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        // Reliza um cast com o event_data
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG_AP,"Dispositivo conectado! MAC ADDR :" 
                        "%02x:%02x:%02x:%02x:%02x:%02x",
                        event->mac[0],event->mac[1],event->mac[2],
                        event->mac[3],event->mac[4],event->mac[5]);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG_AP,"Dispositivo disconectado! MAC ADDR :" 
                        "%02x:%02x:%02x:%02x:%02x:%02x",
                        event->mac[0],event->mac[1],event->mac[2],
                        event->mac[3],event->mac[4],event->mac[5]);
    }
}
void vtask_list_station(void *args)
{
    //Loop
    while(1)
    {
        printf("Monitoramento de Dispositivos conectados\n");
        printf("---------------------------------\n");
        
        wifi_sta_list_t wifi_sta_list;
        //Limpa espaço da memoria reservada para a váriavel
        memset(&wifi_sta_list,0,sizeof(wifi_sta_list));
        //Busca a lista de AP conectado
        esp_wifi_ap_get_sta_list(&wifi_sta_list);
        
        //Caso não encontre nenhum AP, mostra a mensagem abaixo
        if(wifi_sta_list.num <= 0)
        {
            printf("Nenhuma dispositivo conectado\n");

            printf("---------------------------------\n");
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;

        }
        //Percorre a lista de AP conectados
        for(int i = 0; i <wifi_sta_list.num;i++)
        {
            printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x | RSSI : %d\n",
            wifi_sta_list.sta->mac[0],wifi_sta_list.sta->mac[1],wifi_sta_list.sta->mac[2],
            wifi_sta_list.sta->mac[3],wifi_sta_list.sta->mac[4],wifi_sta_list.sta->mac[5],
            wifi_sta_list.sta->rssi);
        }
        printf("---------------------------------\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
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
    ESP_LOGI(TAG_AP, "Iniciando o acess point");
    
    //Criando Task para gerenciar os dispositivos conectados ao ponto de acesoo
    xTaskCreate(vtask_list_station,"list station", 2048, NULL, 5, NULL);
}