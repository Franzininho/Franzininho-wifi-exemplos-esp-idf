/* Header file: wi-fi (station e ap) */

#ifndef HEADER_MODULO_WIFI_ST_AP
#define HEADER_MODULO_WIFI_ST_AP

/* Segurança wi-fi */
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

/* Credenciais wi-fi - station */
#define WIFI_SSID_ST_DEFAULT           CONFIG_ESP_WIFI_ST_SSID
#define WIFI_PASS_ST_DEFAULT           CONFIG_ESP_WIFI_ST_PASSWORD
#define WIFI_ST_STATIC_IP              CONFIG_ESP_WIFI_ST_STATIC_IP
#define WIFI_ST_GATEWAY                CONFIG_ESP_WIFI_ST_GATEWAY
#define WIFI_ST_NETWORK_MASK           CONFIG_ESP_WIFI_ST_NETWORK_MASK
#define TAM_MAX_SSID_ST_WIFI           32
#define TAM_MAX_PASS_ST_WIFI           64
#define TAM_MAX_IP_ESTATICO            16
#define TAM_MAX_IP_GATEWAY             16
#define TAM_MAX_GATEWAY_MASK           16

/* Chaves das credenciais wi-fi e ips na NVS */
#define CHAVE_SSID_WIFI             "ssid"
#define CHAVE_PASS_WIFI             "pass"
#define CHAVE_IP_GATEWAY_WIFI       "ipgtw"
#define CHAVE_GATEWAY_MASK_WIFI     "gtwmk"

#endif

/* Protótipos */
void wifi_init_sta_st(void);
bool get_status_conexao_wifi(void);