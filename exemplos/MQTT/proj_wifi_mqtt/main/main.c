/* Projeto de comunicação simples MQTT com a franzininho wifi e controle via 
 * MQTT dos LEDs da placa
 * 
 * IMPORTANTE: 
 * Este projeto foi desenvolvido usando VSCode (extensão do ESP-IDF), com ESP-IDF versão
 * 4.4.3. Utilize a mesma versão de ESP-IDF para evitar incompatibilidades.
 * 
 * Autor: Pedro Bertoleti
*/

#include <stdio.h>
#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

/* Includes dos módulos */
#include "wifi_st/wifi_st.h"
#include "modulo_mqtt/modulo_mqtt.h"
#include "modulo_principal/modulo_principal.h"
#include "modulo_leds/modulo_leds.h"

void app_main(void)
{   
    /* Inicializa NVS (requerido para o wifi funcionar) */
    nvs_flash_init();

    /* Inicializa LEDs da Franzininho wifi*/
    init_modulo_leds();

    /* Inicializa wifi */
    wifi_init_sta_st();

    /* Inicializa módulo principal */
    init_modulo_principal();
}
