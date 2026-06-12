#include <stdio.h>
#include <SDL2/SDL.h>

#define WIDTH 800
#define HEIGHT 600

//Aloca a tela utilizando o titulo passado como parametro e as largura e altura definidas no tela.h
SDL_Window *criaTela(char *titulo);

//Desenha um segmento de reta na tela
void desenhaArestaTela(SDL_Renderer *renderer, float *ponto1, float *ponto2);

//Desaloca a tela
void desalocaTela(SDL_Window *window);
