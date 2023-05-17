/* Módulo: LEDs 
 * Autor: Pedro Bertoleti
*/

/* Includes */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "modulo_leds.h"

/* Definições - debug */
#define MOD_LEDS_TAG "MOD_LEDS"

/* Variáveis estáticas */
static char estado_led_21 = 0x00;
static char estado_led_33 = 0x00;

/* Função: inicializa modulo de LEDs
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
void init_modulo_leds(void)
{
    gpio_config_t io_conf = {};

    ESP_LOGI(MOD_LEDS_TAG, "Inicializando modulo de LEDs...");

    /* Configura GPIO dos LEDs 33 e 21 e apaga ambos LEDs*/
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_PINSEL_LEDS_21_33;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(GPIO_LED_21, 0);
    gpio_set_level(GPIO_LED_33, 0);
    estado_led_21 = 0x00;
    estado_led_33 = 0x00;

    ESP_LOGI(MOD_LEDS_TAG, "Modulo de LEDs inicializado");
}

/* Função: alterna estado de um LED da franzininho wifi
 * Parâmetros: ID do LED para alterar estado
 * Retorno: nenhum
 */
void toogle_led_franzininho_wifi(int id_led)
{
    switch (id_led)
    {
    case ID_LED_21:
    {
        if (estado_led_21 == 0x00)
        {
            estado_led_21 = 0x01;
        }
        else
        {
            estado_led_21 = 0x00;
        }

        gpio_set_level(GPIO_LED_21, (int)estado_led_21);
        break;
    }

    case ID_LED_33:
    {
        if (estado_led_33 == 0x00)
        {
            estado_led_33 = 0x01;
        }
        else
        {
            estado_led_33 = 0x00;
        }

        gpio_set_level(GPIO_LED_33, (int)estado_led_33);
        break;
    }

    default:
        break;
    }
}

/* Função: faz a leitura do estado de um LED da Franzininho
 * Parâmetros: ID do LED para ler o estado
 * Retorno: 0: LED apagado
 *          1: LED aceso
 */
int le_estado_led_franzininho_wifi(int id_led)
{
    int estado_lido = 0;

    switch (id_led)
    {
    case ID_LED_21:
    {
        estado_lido = (int)estado_led_21;
        break;
    }

    case ID_LED_33:
    {
        estado_lido = (int)estado_led_33;
        break;
    }

    default:
        break;
    }

    return estado_lido;
}