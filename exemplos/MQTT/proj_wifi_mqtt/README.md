# Exemplo de uso de MQTT com a Franzininho wifi (com ESP-IDF)

Aqui encontra-se um exemplo de como usar o MQTT com a placa Franzininho wifi, programado com o ESP-IDF. O exemplo consiste em em controlar os dois LEDs da placa via MQTT, assim como receber via MQTT o status atual dos dois LEDs (a cada 5 segundos).
Este exemplo foi programado usando a versão 4.4.3, com o VSCode (através da extensão do ESP-IDF).


## Como preparar e compilar o projeto?

Para usar o exemplo, basta seguir o procedimento abaixo:

1. Tenha instalado o VSCode e extensão do ESP-IDF para o VSCode (nela, baixar o ESP-IDF versão 4.4.3)
2. No terminal do VSCode (utilize Prompt de Comando), insira o comando *%userprofile%\esp\esp-idf\export.bat*
3. Ainda no prompt de comando, entre no menuconfig com o seguinte comando: *idf.py menuconfig*
4. No menuconfig, vá até *Component config → Configuracao - wifi (station)* e altere o SSID, senha, endereço de gateway e máscara de rede da rede wifi na qual o ESP32 deve se conectar
5. Aperte ESC até aparecer a mensagem confirmando que deseja sair. Informe que deseja sair E salvar apertando a tecla *y*
6. No código-fonte (em modulo_mqtt.h), os tópicos de envio e recepção de dados via MQTT são, respectivamente: /franzininho_wifi/topico_envio e /franzininho_wifi/topico_recepcao. Altere tais tópicos para nomes que sejam particulares do seu projeto, para evitar conflitos com outros que possam estar usando o mesmo projeto-exemplo no mesmo broker MQTT.
7. Ainda no código-fonte (em modulo_mqtt.c), é utilizado o broker MQTT livre / gratuito para testes broker.hivemq.com. Se desejar alterar o broker, porta ou ambos, altere em *config_mqtt*, na função *init_mqtt*.
8. Compile o código-fonte e grave-o conforme instruções da documentação da Franzininho wifi: https://franzininho.github.io/exemplos-esp-idf/


## Como testar o projeto?

Uma vez compilado e gravado, siga o procedimento abaixo para testá-lo:

1. Utilizando um client MQTT qualquer (exemplo: MQTTLens - https://chrome.google.com/webstore/detail/mqttlens/hemojaaeigabkbcookmlgmdigohjobjm?hl=pt), conecte-se ao mesmo broker MQTT que a Franzininho wifi está programada para conectar
2. No client MQTT, subescreva-se ao tópico que a Franzininho wifi usa pra ENVIAR dados (exemplo: /franzininho_wifi/topico_envio). Isso permitirá que o seu client MQTT receba o status dos LEDs da placa a cada 5 segundos.
3. Ainda no client MQTT, para controlar os LEDs da placa, faça o seguinte: envie *0* para alternar o estado (ligado/desligado) do LED 21; envie *1* para alternar o estado (ligado/desligado) do LED 33.