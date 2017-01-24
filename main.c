#include "angry_birds.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

ALLEGRO_DISPLAY *display = NULL; //tela
ALLEGRO_EVENT_QUEUE *event_queue = NULL; //fila de eventos detectados pelo Allegro (ex: tecla que foi apertada, clique do mouse etc)
ALLEGRO_TIMER *timer = NULL; //temporizador: quando FPS = 10, a cada 0.1 segundos o tempo passa de t para t+1 e a fila de eventos detecta
ALLEGRO_BITMAP *bird = NULL; //figura da flecha
ALLEGRO_BITMAP *alvo = NULL; //figura do alvo
ALLEGRO_BITMAP *estilingue= NULL; //figura do alvo
ALLEGRO_BITMAP *fundo = NULL;//Fundo
ALLEGRO_FONT *fonte = NULL; //fonte das letras
ALLEGRO_FONT *fonteMaior = NULL; //fonte das letras
ALLEGRO_AUDIO_STREAM *musica = NULL;
bool redraw = true; //variavel que indica se eh para redesenhar o passaro

int main(void){

   if (!inicializar()){
      return -1;
   }

srand(time(NULL));

char texto1[20], texto2[20], texto3[20];  //variavel do tipo char[] que recebe um texto
int angulo = ANG_INICIAL, velocidade = VEL_INICIAL, pontos = 0; //variavel que recebe o valor de entrada 

float ang_rad = 0.0; //cria variavel de angulo em radianos

float largura_bird =  al_get_bitmap_width(bird); //largura e altura da figura da flecha
float altura_bird =  al_get_bitmap_height(bird);
float largura_alvo =  al_get_bitmap_width(alvo); //largura e altura da figura do alvo
float altura_alvo =  al_get_bitmap_height(alvo);
float largura_estilingue =  al_get_bitmap_width(estilingue); //largura e altura da figura do alvo
float altura_estilingue =  al_get_bitmap_height(estilingue);

float bird_x = POSX_BIRD_INICIAL, bird_y = POSY_BIRD_INICIAL; //posicao x e y inicial da flecha na tela
float alvo_x = POSX_ALVO_INICIAL, alvo_y = POSY_ALVO_INICIAL;
float estilingue_x = POSX_ESTIL_INICIAL, estilingue_y = POSY_ESTIL_INICIAL;

bool quit = false;
bool forca_vento = false;

al_attach_audio_stream_to_mixer(musica, al_get_default_mixer());
al_set_audio_stream_playing(musica, true);// que recebe o stream e falso (0 / false) ou verdadeiro (1 / true) para iniciar ou para sua execução.
al_register_event_source(event_queue, al_get_keyboard_event_source()); //registra que a fila de eventos deve detectar quando uma tecla for pressionada no teclado
al_register_event_source(event_queue, al_get_display_event_source(display)); //registrar mudancas na tela dentro da fila de eventos, isto e, sempre que a tela mudar, um evento ocorrera
al_register_event_source(event_queue, al_get_timer_event_source(timer));   
al_start_timer(timer); //inicia o temporizador	

while(!quit){

imprimeFundo();
imprimeEstilingue(estilingue_x,estilingue_y);
imprimeBird(bird_x,bird_y); //chama funcao para imprimir flecha
imprimeAlvo(alvo_x,alvo_y); //chama função para imprimir flecha

sprintf(texto1, "Angulo: %d", angulo); //cria um texto Angulo : angulo
al_draw_text(fonte, al_map_rgb(255, 255, 255), 40, 20, 0, texto1); //imprime o texto armazenado em texto1 na posicao x=10,y=10 e com a cor rgb(117, 63, 135)
sprintf(texto2, "Velocidade: %d", velocidade); //cria um texto Velocidade : angulo
al_draw_text(fonte, al_map_rgb(255, 255, 255), 40, 80, 0, texto2); //imprime o texto armazenado em texto2 na posicao x=10,y=80 e com a cor rgb(128,200,30)
sprintf(texto3, "Score: %d", pontos); //cria um texto Velocidade : angulo
al_draw_text(fonte, al_map_rgb(255, 255, 255), 1050, 20, 0, texto3); //imprime o texto armazenado em texto2 na posicao x=10,y=80 e com a cor rgb(128,200,30)

al_flip_display(); //reinicializa a tela

ALLEGRO_EVENT ev; //variavel do tipo evento
al_wait_for_event(event_queue, &ev); //a variavel ev recebe o primeiro evento da fila de eventos

   if(ev.type == ALLEGRO_EVENT_TIMER){
	  
	  if((rand() % 2) == 0) //movimento do alvo
	     alvo_x += 1;
      else
         alvo_x -= 1;
      redraw = true; 
	 }						
		
   if(ev.type == ALLEGRO_EVENT_KEY_DOWN) { //se o tipo do evento for uma tecla pressionada
      switch(ev.keyboard.keycode) { //verifica qual tecla foi
         case ALLEGRO_KEY_ESCAPE: //caso aperte esc, sai do loop
            quit = true;
            break;
		 case ALLEGRO_KEY_W: 
            angulo += 1;
            break;
		 case ALLEGRO_KEY_S: 
            angulo -= 1;
            break;
		 case ALLEGRO_KEY_A:
            velocidade -= 1;
            break;
		 case ALLEGRO_KEY_LEFT:
		    bird_x -= 10;
			estilingue_x -= 10;
            break;
         case ALLEGRO_KEY_D: 
            velocidade += 1;
            break;
		 case ALLEGRO_KEY_RIGHT:
		    bird_x += 10;
			estilingue_x += 10;
            break;	
		 case ALLEGRO_KEY_V:
            forca_vento = true;
            break;
         case ALLEGRO_KEY_ENTER: 

			ang_rad = converteAnguloRadiano(angulo);
			bool acertou = false;
			float bird_dx = 0.0;
            float bird_dy = 0.0;
            float gravidade = 9.8;
			float vento = ventoAleatorio();
			float forca = velocidade;
			
			while((bird_x < SCREEN_W) && (bird_y < SCREEN_H)){
               
			   ALLEGRO_EVENT e; //variavel do tipo evento
	           al_wait_for_event(event_queue, &e); //a variavel ev recebe o primeiro evento da fila de eventos
      
	           if((colisao(bird_x, bird_y, alvo_x, alvo_y) == true)){ //verifico houve colisao
                  
				  acertou = true;
				  pontos += 1000;
	              
				  imprimeFundo();
				  imprimeEstilingue(estilingue_x,estilingue_y);
				  imprimeBird(bird_x, bird_y);
                  imprimeAlvo(alvo_x, alvo_y);
				  
                  sprintf(texto1, "Angulo: %d", angulo); //cria um texto Angulo : angulo
                  al_draw_text(fonte, al_map_rgb(255, 255, 255), 40, 20, 0, texto1); //imprime o texto armazenado em texto1 na posicao x=10,y=10 e com a cor rgb(117, 63, 135)
                  sprintf(texto2, "Velocidade: %d", velocidade); //cria um texto Velocidade : angulo
                  al_draw_text(fonte, al_map_rgb(255, 255, 255), 40, 80, 0, texto2); //imprime o texto armazenado em texto2 na posicao x=10,y=80 e com a cor rgb(128,200,30)
                  sprintf(texto3, "Score: %d", pontos); //cria um texto Velocidade : angulo
                  al_draw_text(fonte, al_map_rgb(255, 255, 255), 1050, 20, 0, texto3); //imprime o texto armazenado em texto2 na posicao x=10,y=80 e com a cor rgb(128,200,30)	                  
				  
				  al_draw_text(fonteMaior, al_map_rgb(130, 90, 65), 500, 300, 0, "Acertou!"); //cria um texto informativo
				  
				  al_flip_display(); //reinicializa a tela
   
                  al_rest(3.0); //pausa a tela por 3.0 segundos
				  
				  forca_vento = false;
				  
				  angulo = ANG_INICIAL, velocidade = VEL_INICIAL, ang_rad = 0.0;
     			  bird_x = POSX_BIRD_INICIAL, bird_y = POSY_BIRD_INICIAL, alvo_x = alvoAleatorio(), alvo_y = POSY_ALVO_INICIAL;
                  estilingue_x = POSX_ESTIL_INICIAL, estilingue_y = POSY_ESTIL_INICIAL;
				  
				  break;
			   }
			   
			    if(bird_y >= CHAO){ //nao passa do chao
				   acertou = false;
				   bird_y = 600;
				   break;
				}
			  

			   if(e.type == ALLEGRO_EVENT_TIMER) { //se for um evento de timer, ou seja, se foi o tempo que passou de t para t+1
 
                  if((rand() % 2) == 0) //movimento do alvo
	                 alvo_x += 1;
                  else
                     alvo_x -= 1;
     	
				  bird_dx = posicao_x(forca, ang_rad);
		          bird_x += bird_dx; //incrementa as posicoes x e y do passaro com o seu deslocamento dx e dy
                  bird_dy = posicao_y(velocidade, ang_rad);//variacao de x e y ao longo do tempo
				  bird_y += bird_dy;
				  
		          velocidade -= gravidade;
                  if(forca_vento == true)
				     forca -= vento;
				  				     
				  redraw = true;    //como eu movi o passaro, preciso redesenhar ele (remova essa linha e veja o que acontece)
		        }
			  
			   if(redraw && al_is_event_queue_empty(event_queue)) { //se for para eu redesenhar a tela e nao tenho mais nenhum evento para ler
         
                 redraw = false; //nao preciso redesenhar
        
                 imprimeFundo();
				 imprimeEstilingue(estilingue_x,estilingue_y);
                 imprimeBird(bird_x, bird_y); //desenho o passaro na nova posicao
		         imprimeAlvo(alvo_x, alvo_y); //desenho o alvo novamente
				 
				 sprintf(texto1, "Angulo: %d", angulo); //cria um texto Angulo : angulo
                 al_draw_text(fonte, al_map_rgb(255, 255, 255), 40, 20, 0, texto1); //imprime o texto armazenado em texto1 na posicao x=10,y=10 e com a cor rgb(117, 63, 135)
                 sprintf(texto2, "Velocidade: %d", velocidade); //cria um texto Velocidade : angulo
                 al_draw_text(fonte, al_map_rgb(255, 255, 255), 40, 80, 0, texto2); //imprime o texto armazenado em texto2 na posicao x=10,y=80 e com a cor rgb(128,200,30)
                 sprintf(texto3, "Score: %d", pontos); //cria um texto Velocidade : angulo
                 al_draw_text(fonte, al_map_rgb(255, 255, 255), 1050, 20, 0, texto3); //imprime o texto armazenado em texto2 na posicao x=10,y=80 e com a cor rgb(128,200,30)

          		 al_flip_display(); //dou um refresh na tela
               }
			   
	        }//fim while  

            if (!acertou){ 

              imprimeFundo();
			  imprimeEstilingue(estilingue_x,estilingue_y);
			  imprimeBird(bird_x, bird_y);			   
              imprimeAlvo(alvo_x, alvo_y);
              
			  sprintf(texto1, "Angulo: %d", angulo); //cria um texto Angulo : angulo
              al_draw_text(fonte, al_map_rgb(255, 255, 255), 40, 20, 0, texto1); //imprime o texto armazenado em texto1 na posicao x=10,y=10 e com a cor rgb(117, 63, 135)
              sprintf(texto2, "Velocidade: %d", velocidade); //cria um texto Velocidade : angulo
              al_draw_text(fonte, al_map_rgb(255, 255, 255), 40, 80, 0, texto2); //imprime o texto armazenado em texto2 na posicao x=10,y=80 e com a cor rgb(128,200,30)
              sprintf(texto3, "Score: %d", pontos); //cria um texto Velocidade : angulo
              al_draw_text(fonte, al_map_rgb(255, 255, 255), 1050, 20, 0, texto3); //imprime o texto armazenado em texto2 na posicao x=10,y=80 e com a cor rgb(128,200,30)			  
			  
			  al_draw_text(fonteMaior, al_map_rgb(0, 0, 0), 550, 300, 0, "Errou!"); //cria um texto informativo
              
              al_flip_display(); //atualiza a tela

              al_rest(3.0); //pausa a tela por 3.0 segundos
              
			  forca_vento = false;
			  angulo = ANG_INICIAL, velocidade = VEL_INICIAL, ang_rad = 0.0;
     		  bird_x = POSX_BIRD_INICIAL, bird_y = POSY_BIRD_INICIAL, alvo_x = alvoAleatorio(), alvo_y = POSY_ALVO_INICIAL;
              estilingue_x = POSX_ESTIL_INICIAL, estilingue_y = POSY_ESTIL_INICIAL;
		    }   
	    }//fim switch
    }//fim do if
}//fim while principal

finalizar();

return 0;
}

void imprimeBird(float x, float y){

   al_draw_bitmap(bird, x, y, 0);
}

void imprimeAlvo(float x, float y){

   al_draw_bitmap(alvo, x, y, 0);
}

void imprimeEstilingue(float x, float y){

   al_draw_bitmap(estilingue, x, y, 0);
}

void imprimeFundo(){

   al_draw_bitmap(fundo, 0, 0, 0);

}

void finalizar(){

al_destroy_display(display);
al_destroy_timer(timer);
al_destroy_bitmap(bird);
al_destroy_bitmap(alvo);
al_destroy_bitmap(estilingue);
al_destroy_bitmap(fundo);
al_destroy_font(fonte);
al_destroy_font(fonteMaior);
al_destroy_event_queue(event_queue);
al_destroy_audio_stream(musica);

}

bool inicializar() {

   if(!al_init()) { //inicializa o allegro. Se nao conseguir, imprime na tela uma msg de erro.
      fprintf(stderr, "Falha ao inicializar a Allegro.\n");
      return false;
   }

   timer = al_create_timer(1.0 / FPS); //inicializa o temporizador com a taxa de 1 quadro a cada 0.1 segundos. Se nao conseguir, imprime na tela uma msg de erro.
   if(!timer) {
      fprintf(stderr, "Falha ao inicializar o temporizador.\n");
      return false;
   }
   
   display = al_create_display(SCREEN_W, SCREEN_H); //inicializa a tela. Se nao conseguir, imprime na tela uma msg de erro.
   if(!display) {
      fprintf(stderr, "Falha ao inicializar a tela.\n");
      al_destroy_timer(timer);
      return false;
   }
   
   al_set_window_title(display, "Angry Birds");
   
   al_init_image_addon(); //inicializar o modulo de imagens
   bird = al_load_bitmap("bird.png");    //carregar a figura .jpg na variavel flecha
   if(!bird) { //se nao conseguiu achar o arquivo, imprime uma msg de erro
      fprintf(stderr, "Falha ao inicializar imagem do bird.\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return false;
   }

   alvo = al_load_bitmap("alvo.png"); //carregar a figura .jpg na variavel alvo
   if(!alvo) { //se nao conseguiu achar o arquivo, imprime uma msg de erro
      fprintf(stderr, "Falha ao inicializar imagem do alvo.\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
	  al_destroy_bitmap(bird);
      return false;
   }
   
   estilingue = al_load_bitmap("estilingue.png"); //carregar a figura .jpg na variavel alvo
   if(!estilingue) { //se nao conseguiu achar o arquivo, imprime uma msg de erro
      fprintf(stderr, "Falha ao inicializar imagem do estilingue.\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
	  al_destroy_bitmap(bird);
	  al_destroy_bitmap(alvo);
      return false;
   }
   
   fundo = al_load_bitmap("fundo.png"); //carregar a figura .jpg na variavel alvo
   if(!fundo) { //se nao conseguiu achar o arquivo, imprime uma msg de erro
      fprintf(stderr, "Falha ao inicializar imagem de fundo.\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
	  al_destroy_bitmap(bird);
	  al_destroy_bitmap(alvo);
	  al_destroy_bitmap(estilingue);
      return false;
   }
   
   if (!al_install_keyboard()) { //inicializar o modulo de teclado,se nao conseguiu inicializar, imprime uma msg de erro
       fprintf(stderr, "Falha ao inicializar o modulo de teclado.\n");
       return false;
   }
    
	al_init_font_addon(); //inicializar o modulo fonte
	al_init_ttf_addon(); //inicializa o modulo allegro que entende arquivos tff de fontes
    fonte = al_load_font("angrybirds.ttf", 50, 10); //carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    if (!fonte) {
        fprintf(stderr, "Falha ao carregar fonte angrybirds.ttf.\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
	    al_destroy_bitmap(bird);
	    al_destroy_bitmap(alvo);
		al_destroy_bitmap(estilingue);
		al_destroy_bitmap(fundo);
        return false;
   }

   fonteMaior = al_load_font("angrybirds.ttf", 100, 10); //carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
   if (!fonteMaior) {
       fprintf(stderr, "Falha ao carregar fonte angrybirds.ttf.\n");
       al_destroy_display(display);
       al_destroy_timer(timer);
	   al_destroy_bitmap(bird);
	   al_destroy_bitmap(alvo);
	   al_destroy_bitmap(estilingue);
	   al_destroy_bitmap(fundo);
	   al_destroy_font(fonte);
       return false;
    }	
   
   event_queue = al_create_event_queue(); //criar a fila de eventos

   if(!event_queue) { //se nao conseguiu criar a fila de eventos, imprime uma msg de erro
      fprintf(stderr, "Falha ao inicializar fila de eventos.\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
	  al_destroy_bitmap(bird);
	  al_destroy_bitmap(alvo);
	  al_destroy_bitmap(estilingue);
	  al_destroy_bitmap(fundo);
      al_destroy_font(fonte);
	  al_destroy_font(fonteMaior);
      return false;
   }
   
   if (!al_install_audio()) { //responsável por inicializar as funções relativas ao áudi
      fprintf(stderr, "Falha ao inicializar áudio.\n");
      return false;
    }
	
   if (!al_init_acodec_addon()) { //inicializa os codecs necessários para carregar os diversos formatos de arquivo suportados pela Allegro.
       fprintf(stderr, "Falha ao inicializar codecs de áudio.\n");
       return false;
   }
	
   if (!al_reserve_samples(1)) { //recebe a quantidade de canais que devem ser alocados ao mixer principal.
      fprintf(stderr, "Falha ao alocar canais de áudio.\n");
      return false;
   }
	
   musica = al_load_audio_stream("musica.ogg", 4, 1024);
   if (!musica) {
      fprintf(stderr, "Falha ao carregar audio.\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
	  al_destroy_bitmap(bird);
	  al_destroy_bitmap(alvo);
	  al_destroy_bitmap(estilingue);
	  al_destroy_bitmap(fundo);
	  al_destroy_font(fonte);
	  al_destroy_font(fonteMaior);
	  al_destroy_event_queue(event_queue);
      return false;
   }
	
return true;
}