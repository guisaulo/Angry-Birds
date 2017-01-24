#include "angry_birds.h"
#include <math.h>

float converteAnguloRadiano(float angulo){

   return (angulo * M_PI) / 180.0;

}

float ventoAleatorio(void){

   return 1 + rand() % 10;
   
}

float alvoAleatorio(void){
 
   return 180 + rand() % 600;

}

float posicao_x(float forca, float ang_rad){

   return forca * cos(ang_rad);

}

float posicao_y(float velocidade, float ang_rad){
 
   return (-1.0) * velocidade * sin(ang_rad); 
   
}

bool colisao(float x1, float y1, float x2, float y2){

   if ((x1 > x2 + 80) || (y1 > y2 + 80) || (x1 + 60 < x2) || (y1 + 60 < y2))
      return false;
   else
      return true;
}