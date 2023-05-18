/* Header file: módulo de LEDs */

#ifndef HEADER_MODULO_LEDS
#define HEADER_MODULO_LEDS

/* Definições - GPIOs dos LEDs */
#define GPIO_LED_21      21
#define GPIO_LED_33      33

/* Definições - IDs dos LEDs */
#define ID_LED_21        '0'
#define ID_LED_33        '1'

/* Definição - máscara para configuração dos GPIOs dos LEDS */
#define GPIO_PINSEL_LEDS_21_33    ( (1ULL<<GPIO_LED_21) | (1ULL<<GPIO_LED_33) )

#endif

/* Protótipos */
void init_modulo_leds(void);
void toogle_led_franzininho_wifi(int id_led);
int le_estado_led_franzininho_wifi(int id_led);
