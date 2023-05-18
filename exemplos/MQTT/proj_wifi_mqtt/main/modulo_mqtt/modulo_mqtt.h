/* Header file: módulo MQTT */

#ifndef HEADER_MODULO_MQTT
#define HEADER_MODULO_MQTT

/* Definições - tópicos MQTT */
#define MQTT_TOPICO_ENVIO     "/franzininho_wifi/topico_envio"
#define MQTT_TOPICO_RECEPCAO  "/franzininho_wifi/topico_recepcao"

/* Definições - comunicação MQTT */
#define QOS_UTILIZADO               0
#define CALCULA_TAM_MSG_PUBLICADA   0
#define FLAG_RETAIN                 0

/* Definições - sucesso ou falha */
#define FALHA_ENVIO_MSG_MQTT       -1

/* Definição - tamanho máximo da string para publicar */
#define MAX_STR_PARA_PUBLICAR      100

#endif

/* Protótipos */
void init_mqtt(void);
int publica_mqtt(char * pt_str_para_publicar);
bool verifica_se_mqtt_esta_conectado(void);