#include <stdbool.h>

#define FPS 10 //numero de quadros por segundo, a ser carregado no temporizador (altere esse valor e veja o que acontece)
#define ANG_INICIAL 50
#define VEL_INICIAL 80
#define POSX_BIRD_INICIAL 100
#define POSY_BIRD_INICIAL 495
#define POSX_ALVO_INICIAL 1200
#define POSY_ALVO_INICIAL 580
#define POSX_ESTIL_INICIAL 100
#define POSY_ESTIL_INICIAL 508
#define SCREEN_H 768
#define SCREEN_W 1366
#define CHAO  560



void imprimeBird(float x, float y);

void imprimeAlvo(float x, float y);

void imprimeFundo(void);

void imprimeEstilingue(float x, float y);

float converteAnguloRadiano(float angulo);

float ventoAleatorio(void);

float alvoAleatorio(void);

float posicao_x(float forca, float ang_rad);

float posicao_y(float velocidade, float ang_rad);

bool colisao(float x1, float y1, float x2, float y2);

void finalizar();

bool inicializar();

