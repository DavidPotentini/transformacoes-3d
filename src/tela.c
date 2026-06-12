#include <stdio.h>
#include <SDL2/SDL.h>
#include "tela.h"

//Aloca a tela utilizando o titulo passado como parametro e as largura e altura definidas no tela.h
SDL_Window *criaTela(char *titulo){
    return SDL_CreateWindow( titulo, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
}

//Desenha um segmento de reta na tela
void desenhaArestaTela(SDL_Renderer *renderer, float *ponto1, float *ponto2){
    int p1x, p1y, p2x, p2y;

    p1x = ((ponto1[0])+1) * WIDTH/2;
    p1y = (1-ponto1[1]) * HEIGHT/2;
    p2x = ((ponto2[0])+1) * WIDTH/2;
    p2y = (1-ponto2[1]) * HEIGHT/2;

    SDL_RenderDrawLine(renderer, p1x, p1y, p2x, p2y);

    return;
}

//Desaloca a tela
void desalocaTela(SDL_Window *window){
    SDL_DestroyWindow(window);
}
