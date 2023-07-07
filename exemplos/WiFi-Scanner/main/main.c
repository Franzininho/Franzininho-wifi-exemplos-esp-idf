#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
//Definindo número máx de APs
#define MAX_APs 20




// Função que busca qual tipo de autenticação a rede opera
static char* getAuthModeName(wifi_auth_mode_t auth_mode)
{
  char *names[] = {"OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "MAX"};
 return names[auth_mode];
}


void app_main()
{
 // Inicializa o NVS
 ESP_ERROR_CHECK(nvs_flash_init());
 ESP_ERROR_CHECK(esp_event_loop_create_default());
 esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
 assert(sta_netif);


 //Configura o driver WiFi em modo Station
 wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
 ESP_ERROR_CHECK(esp_wifi_init(&cfg));
 ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
 //Inicializa o driver WiFi
 ESP_ERROR_CHECK(esp_wifi_start());
  // Configurando a execução do scanner
 wifi_scan_config_t scan_config =
 {
   .ssid = 0,
   .bssid = 0,
   .channel = 0,
   .show_hidden = true
 };


 printf("Iniciando Scanner da Rede...");
 //Iniciando o scanner da rede
 ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
 printf(" Finalizou !\n");
 printf("\n");
  
 uint16_t ap_num = MAX_APs;
 wifi_ap_record_t ap_records[MAX_APs];
 ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));
  // print the list
 printf("Encontrou %d Pontos de Acesso:\n", ap_num);
 printf("\n");
 printf("               SSID              | Channel | RSSI |   Auth Mode \n");
 printf("----------------------------------------------------------------\n");
 for(int i = 0; i < ap_num; i++)
   printf("%32s | %7d | %4d | %12s\n", (char *)ap_records[i].ssid, ap_records[i].primary, ap_records[i].rssi, getAuthModeName(ap_records[i].authmode));
 printf("----------------------------------------------------------------\n");
  while(1)
 {
   vTaskDelay(1000 / portTICK_PERIOD_MS); 
 }
}
