#ifndef RELAY_COMPONENT_H
#define RELAY_COMPONENT_H

// Estrutura para armazenar as características do relé
typedef struct {
    int pin;        // Pino do relé
    int state;      // Estado do relé (ligado ou desligado)
} Relay;

// Função de inicialização do componente relé
void relay_init(Relay *relay, int pin);

// Função para ligar o relé
void relay_turn_on(Relay *relay);

// Função para desligar o relé
void relay_turn_off(Relay *relay);

// Função para obter o status do relé
int relay_get_status(Relay *relay);

#endif /* RELAY_COMPONENT_H */
