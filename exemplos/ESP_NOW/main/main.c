#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "esp_crc.h"

#define CONFIG_MASTER  0X01
#define CONFIG_SLAVE   0X02
#define ESPNOW_MAXDELAY 512  
char * TAG = "FRANZININHO ESP-NOW";

QueueHandle_t xQueue_esp_now = NULL;

uint8_t mac_addr_send[ESP_NOW_ETH_ALEN];
uint8_t mac_addr_recv[ESP_NOW_ETH_ALEN];

typedef enum
{
    SEND_DATA_CB,
    RECV_DATA_CB,
}esp_now_event_t;
esp_now_event_t  event; 

static void wifi_init()
{
    esp_err_t ret;
   
    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t config_wifi = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&config_wifi);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_AP); // Configura como Acess Point
    esp_wifi_start(); // Inicializa o WiFi de acordo com as configurações realizadas

}

static void send_data_cb(const uint8_t* mac_addr, esp_now_send_status_t status)
{
    
    if(mac_addr == NULL) 
    {
        ESP_LOGI(TAG,"Agumento errado - send_data_cb");
        return ;
    }
    
    event  = SEND_DATA_CB;
    memcpy(mac_addr_send,mac_addr,ESP_NOW_ETH_ALEN);

    if(xQueueSend(xQueue_esp_now, &event, ESPNOW_MAXDELAY)!= pdTRUE)
    {
        ESP_LOGW(TAG, "Erro ao enviar para a fila");
    }
    
    

}

uint8_t  *current_data ;
uint8_t  len_data ;
void recv_data_cb (const uint8_t* mac_addr,const uint8_t* data ,int len)
{
    if(mac_addr == NULL || data == NULL || len <= 0)
        return;
    event = RECV_DATA_CB;
    memcpy(mac_addr_recv,mac_addr,ESP_NOW_ETH_ALEN);
    current_data = malloc(len);
    memcpy(current_data, data, len);
    len_data  = len;

    if(xQueueSend(xQueue_esp_now,&event,ESPNOW_MAXDELAY)!= pdTRUE)
    {
         ESP_LOGW(TAG, "Erro ao enviar para a fila");
         free(current_data);
    }
}

void app_main(void)
{
    xQueue_esp_now = xQueueCreate(6,sizeof(int));
}
