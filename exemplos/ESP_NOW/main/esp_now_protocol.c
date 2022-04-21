#include "esp_now_protocol.h"
#include "time.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"


#define MAX_LEN_ADDR 6
#define HELLO_MSG "Fala Franzininho 1"
#define IDLE 0
#define SEND_MSG 1
#define RECV_MSG 2
#define SEND_HELLO 3

// 7C:DF:A1:05:D0:80 -- Escravo
// 7C:DF:A1:17:65:E6 -- Mestre
//#define MASTER
char * TAG = "FRANZININHO ESP-NOW";
uint8_t broadcast_addr[MAX_LEN_ADDR]    = {0x7C, 0xDF,0xA1,0x05,0xD0,0x80};   // Endereço MAC 
uint8_t master_addr [MAX_LEN_ADDR]      = {0x7C, 0xDF,0xA1,0X17,0X65,0XE6};

esp_now_peer_info_t peer_info;
char* recv_data = 0;
uint8_t event = 0;

uint64_t millis()
{
    return (esp_timer_get_time()/1000);
}


void wifi_init()
{
   	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
	;
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
}



static void esp_now_send_data(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //ESP_LOGI(TAG,"Ultimo pacote enviado para MAC: %s",macStr);

}

static void esp_now_recv_data(const uint8_t *mac, const uint8_t *data, int len) 
{
 
    ESP_LOGI(TAG, "Recebido do MAC:");
    for (int i = 0; i < MAX_LEN_ADDR; i++) 
    {
    
        printf("%02X", mac[i]); 
        if (i < 5)printf(":");
    } 

    recv_data  = (char*)data;
    if(!strcmp(recv_data, HELLO_MSG))
    {
        event = 1;
    }
    ESP_LOGI(TAG, "Dado Recebido %s | Tamanho do dado  %d",recv_data, len);

}

void esp_send_my_msg(char *msg){
    #ifdef MASTER
    esp_err_t result = esp_now_send(broadcast_addr, (uint8_t *)msg, 250);
    #else
    esp_err_t result = esp_now_send(master_addr, (uint8_t *)msg, 250);
    #endif

    if (result != ESP_OK)
        ESP_LOGI(TAG, "Erro ao enviar o dado");
    else
        ESP_LOGI(TAG, "Dado enviado por sucesso");
  
}


void esp_now_reinit()
{
    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
    ESP_ERROR_CHECK( esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_set_channel(6, 0) );
    if (esp_now_init() != ESP_OK) {
        ESP_LOGI(TAG, "Error at Espnow init");
        return;
    }
    else{
        ESP_LOGI(TAG, "Espnow init OK");
    }

    memcpy(peer_info.peer_addr, master_addr, 6);
    peer_info.channel = 0;  
    peer_info.encrypt = false;

    if (esp_now_add_peer(&peer_info) != ESP_OK){
    ESP_LOGI(TAG, "Error adding peer");
    return;
    }
    else{
        ESP_LOGI(TAG, "Peer info added");
    }

    esp_now_register_recv_cb(esp_now_recv_data);
    esp_now_register_send_cb(esp_now_send_data);

}

void vTask_esp_now_franz_1(void *pvParameters)
{
    
    while(1)
    {
        switch (event)
        {
            case IDLE:
                ESP_LOGI(TAG, "Esperando alguma mensagem...");
                
                break;
            case SEND_MSG:
                esp_send_my_msg("Fala Franzininho 2, tudo certo? ");
                event = IDLE;
                break;
            
            default:
                break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTask_esp_now_franz_2(void *pvParameters)
{
    int random_num = 0;
    uint64_t time_2_hello = 0;
    char number[10] = {};
    event = SEND_MSG;
    while(1)
    {
        random_num = rand();
        switch (event)
        {
            
            case SEND_MSG:
                
                sprintf(number, "%d",10);
                esp_send_my_msg(number);
                
                break;
            case SEND_HELLO:
                esp_send_my_msg(HELLO_MSG);
                event = SEND_MSG;
                break;
            default:
                break;
        }
        
        if(millis() - time_2_hello > 5000)
        {
            event = SEND_HELLO;
            time_2_hello = millis();
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void esp_now_protocol_init()
{
    
    esp_err_t ret ;
    
    ret = esp_now_init();
    if(ret != ESP_OK)
    {
        ESP_LOGI(TAG,"Erro ao inicializar o protocolo esp now");
        return;
    }
    ESP_LOGI(TAG,"esp now inicializado");
#ifdef MASTER
    memcpy(peer_info.peer_addr, broadcast_addr, MAX_LEN_ADDR);
    peer_info.channel = 0;  
    peer_info.encrypt = false;
	peer_info.ifidx = ESP_IF_WIFI_STA;
#else
    memcpy(peer_info.peer_addr, master_addr, MAX_LEN_ADDR);
    peer_info.channel = 0;  
    peer_info.encrypt = false;
    peer_info.ifidx = ESP_IF_WIFI_STA;
#endif
    ret = esp_now_add_peer(&peer_info);
    if(ret != ESP_OK)
    {
        ESP_LOGI(TAG,"Erro ao adicionar dispostivo para pareamento");
        return;
    }
    ESP_LOGI(TAG,"Dispositivo adicionado");
    
    // Registrando as funções de callback
    esp_now_register_recv_cb(esp_now_recv_data);
    esp_now_register_send_cb(esp_now_send_data);
    // Criando task de controle 
#ifdef MASTER
    xTaskCreate( vTask_esp_now_franz_1, "esp_now", 4096, NULL, 2, NULL);
#else
    xTaskCreate( vTask_esp_now_franz_2, "esp_now", 4096, NULL, 2, NULL);
#endif

}
