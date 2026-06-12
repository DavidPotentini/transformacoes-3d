#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "objeto.h"
#include "camera.h"
#include "tela.h"
#include "algebra.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void rotacionaEixo(float *v, float *k, float ang){
    float c = cosf(ang), s = sinf(ang);
    float kx = k[1]*v[2] - k[2]*v[1];   //k x v
    float ky = k[2]*v[0] - k[0]*v[2];
    float kz = k[0]*v[1] - k[1]*v[0];
    float kv = k[0]*v[0] + k[1]*v[1] + k[2]*v[2];   //k . v
    float rx = v[0]*c + kx*s + k[0]*kv*(1-c);
    float ry = v[1]*c + ky*s + k[1]*kv*(1-c);
    float rz = v[2]*c + kz*s + k[2]*kv*(1-c);
    v[0] = rx; v[1] = ry; v[2] = rz;
}

//Desenha um objeto na tela aplicando a modelMatrix do objeto e a matriz de
//visualizacao (viewMatrix) recebida como parametro.
void desenhaObjetoTela(SDL_Renderer *renderer, float **matriz, tObjeto3d *objeto, float zoom){
    int i;
    for(i = 0; i < objeto->nArestas; i++){
        int ia = objeto->arestas[i][0];
        int ib = objeto->arestas[i][1];

        //Aplica a modelMatrix (transformacoes do objeto)
        float *ma = multMatriz4dPonto(objeto->modelMatrix, objeto->pontos[ia]);
        float *mb = multMatriz4dPonto(objeto->modelMatrix, objeto->pontos[ib]);

        //Aplica a viewMatrix (transformacoes da camera)
        float *va = multMatriz4dPonto(matriz, ma);
        float *vb = multMatriz4dPonto(matriz, mb);

        //Zoom ortografico: escala as coordenadas projetadas x,y
        va[0] *= zoom; va[1] *= zoom;
        vb[0] *= zoom; vb[1] *= zoom;

        //Projecao ortogonal: desenha usando as coordenadas x e y resultantes
        desenhaArestaTela(renderer, va, vb);

        free(ma); free(mb); free(va); free(vb);
    }
}

//Desenha um chao de referencia: uma grade no plano XZ a uma altura fixa Y.
void desenhaChao(SDL_Renderer *renderer, float **matriz, float zoom){
    const float Y = -0.4f;     //altura do chao (logo abaixo do objeto centrado)
    const float R = 1.0f;      //meia-extensao da grade
    const float passo = 0.2f;  //espacamento entre as linhas
    float t;
    for(t = -R; t <= R + 0.0001f; t += passo){
        //Linha paralela ao eixo Z (x fixo) e linha paralela ao eixo X (z fixo)
        float a[4] = {  t, Y, -R, 1.0f }, b[4] = {  t, Y,  R, 1.0f };
        float c[4] = { -R, Y,  t, 1.0f }, d[4] = {  R, Y,  t, 1.0f };
        float *pa = multMatriz4dPonto(matriz, a);
        float *pb = multMatriz4dPonto(matriz, b);
        float *pc = multMatriz4dPonto(matriz, c);
        float *pd = multMatriz4dPonto(matriz, d);
        pa[0] *= zoom; pa[1] *= zoom; pb[0] *= zoom; pb[1] *= zoom;
        pc[0] *= zoom; pc[1] *= zoom; pd[0] *= zoom; pd[1] *= zoom;
        desenhaArestaTela(renderer, pa, pb);
        desenhaArestaTela(renderer, pc, pd);
        free(pa); free(pb); free(pc); free(pd);
    }
}


int main( int argc, char * argv[] ){

    if (SDL_Init( SDL_INIT_EVERYTHING) < 0){
        printf("SDL nao inicializou! SDL Erro: %s\n", SDL_GetError());
    }

    SDL_Window *window = criaTela("Trabalho 2 - Transformacoes 3D");

    if(window == NULL){
        printf("SDL nao criou a janela! SDL Erro: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    //Carrega o objeto a partir do arquivo (.dcg). Permite informar outro pelo argv.
    char *arquivo = (argc > 1) ? argv[1] : "objetos/cubo.dcg";
    tObjeto3d *objeto = carregaObjeto(arquivo);
    if(objeto == NULL){
        printf("Nao foi possivel carregar o objeto \"%s\".\n", arquivo);
        desalocaTela(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    //Cria e posiciona a camera. Guardo a posicao em variaveis para poder
    //altera-la em tempo de execucao e recalcular a viewMatrix.
    float camX = 0, camY = 0, camZ = 1;   //posicao da camera
    float focX = 0, focY = 0, focZ = 0;    //ponto para onde a camera olha
    tCamera3d *camera = criaCamera();
    defineCamera(camera, camX, camY, camZ,   focX, focY, focZ,   0, 1, 0);

    //Centraliza o objeto na origem usando o centroide (media dos vertices).
    int k;
    float cx = 0, cy = 0, cz = 0;
    for(k = 0; k < objeto->nPontos; k++){
        cx += objeto->pontos[k][0];
        cy += objeto->pontos[k][1];
        cz += objeto->pontos[k][2];
    }
    cx /= objeto->nPontos; cy /= objeto->nPontos; cz /= objeto->nPontos;
    transladaObjeto(objeto, -cx, -cy, -cz);
    escalaObjeto(objeto, 0.08, 0.08, 0.08);

    imprimeObjetoDBG(objeto);
    printf("\nControles:\n");
    printf("  Setas        -> rotaciona em X (cima/baixo) e Y (esq/dir)\n");
    printf("  Q / E        -> rotaciona em Z\n");
    printf("  W A S D      -> translada na tela\n");
    printf("  M / N        -> escala (aumenta / diminui)\n");
    printf("  --- camera (viewMatrix): orbita ao redor do objeto ---\n");
    printf("  Arrastar mouse (botao esq.) -> orbita a camera\n");
    printf("  Roda do mouse -> aproxima / afasta (zoom)\n");
    printf("  R            -> reseta a camera para a vista frontal\n");
    printf("  P            -> imprime o objeto no terminal\n");
    printf("  ESC          -> sai\n\n");

    float cimaX = 0.0f, cimaY = 1.0f, cimaZ = 0.0f;  //vetor "cima" da camera
    const float SENS = 0.01f;        //sensibilidade do mouse (rad/pixel)
    float zoom = 1.0f;               //fator de zoom ortografico (roda do mouse)

    SDL_Event windowEvent;

    while(1){
        if( SDL_PollEvent(&windowEvent)){
            if(windowEvent.type == SDL_QUIT){
                break;
            }
            if(windowEvent.type == SDL_KEYDOWN){
                switch(windowEvent.key.keysym.sym){
                    case SDLK_ESCAPE:                                       goto fim;
                    case SDLK_UP:    rotacionaObjetoEixoX(objeto,  3);       break;
                    case SDLK_DOWN:  rotacionaObjetoEixoX(objeto, -3);       break;
                    case SDLK_LEFT:  rotacionaObjetoEixoY(objeto, -3);       break;
                    case SDLK_RIGHT: rotacionaObjetoEixoY(objeto,  3);       break;
                    case SDLK_q:     rotacionaObjetoEixoZ(objeto,  3);       break;
                    case SDLK_e:     rotacionaObjetoEixoZ(objeto, -3);       break;
                    case SDLK_w:     transladaObjeto(objeto, 0,  0.03, 0);   break;
                    case SDLK_s:     transladaObjeto(objeto, 0, -0.03, 0);   break;
                    case SDLK_a:     transladaObjeto(objeto, -0.03, 0, 0);   break;
                    case SDLK_d:     transladaObjeto(objeto,  0.03, 0, 0);   break;

                    //Escala pelo teclado: M aumenta, N diminui
                    case SDLK_m:     escalaObjeto(objeto, 1.1, 1.1, 1.1);     break;
                    case SDLK_n:     escalaObjeto(objeto, 0.9, 0.9, 0.9);     break;

                    case SDLK_p:     imprimeObjetoDBG(objeto);               break;

                    //R = reseta a camera para a vista frontal inicial.
                    case SDLK_r:
                        camX = 0; camY = 0; camZ = 1;
                        cimaX = 0; cimaY = 1; cimaZ = 0;
                        defineCamera(camera, camX, camY, camZ, focX, focY, focZ, 0, 1, 0);
                        break;
                }
            }

            //Zoom ortografico pela roda do mouse: roda para cima aproxima,
            //para baixo afasta.
            if(windowEvent.type == SDL_MOUSEWHEEL){
                if(windowEvent.wheel.y > 0)      zoom *= 1.1f;
                else if(windowEvent.wheel.y < 0) zoom /= 1.1f;
                if(zoom < 0.1f)  zoom = 0.1f;
                if(zoom > 10.0f) zoom = 10.0f;
            }

            //--- Transformacoes da CAMERA (viewMatrix): ORBITA pelo mouse ---
            if(windowEvent.type == SDL_MOUSEMOTION &&
               (windowEvent.motion.state & SDL_BUTTON_LMASK)){
                float yaw   = -windowEvent.motion.xrel * SENS;
                float pitch = -windowEvent.motion.yrel * SENS;

                //Vetor da camera relativo ao foco e vetor "cima" atuais.
                float off[3]  = { camX - focX, camY - focY, camZ - focZ };
                float cima[3] = { cimaX, cimaY, cimaZ };

                //Eixos locais: n (tras, normalizado), u (direita), v (cima verdadeiro).
                float n[3] = { off[0], off[1], off[2] };
                float ln = sqrtf(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
                n[0] /= ln; n[1] /= ln; n[2] /= ln;
                float u[3] = { cima[1]*n[2] - cima[2]*n[1],
                               cima[2]*n[0] - cima[0]*n[2],
                               cima[0]*n[1] - cima[1]*n[0] };
                float lu = sqrtf(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);
                u[0] /= lu; u[1] /= lu; u[2] /= lu;
                float v[3] = { n[1]*u[2] - n[2]*u[1],
                               n[2]*u[0] - n[0]*u[2],
                               n[0]*u[1] - n[1]*u[0] };

                //Giro horizontal em torno de v (cima) e vertical em torno de u
                //(direita). Roda a posicao e o "cima" pelos mesmos eixos: a base
                rotacionaEixo(off,  v, yaw);   rotacionaEixo(cima, v, yaw);
                rotacionaEixo(off,  u, pitch); rotacionaEixo(cima, u, pitch);

                camX = focX + off[0];
                camY = focY + off[1];
                camZ = focZ + off[2];
                cimaX = cima[0]; cimaY = cima[1]; cimaZ = cima[2];

                defineCamera(camera, camX, camY, camZ, focX, focY, focZ,
                             cimaX, cimaY, cimaZ);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //Chao de referencia em cinza
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
        desenhaChao(renderer, camera->viewMatrix, zoom);

        //Objeto em branco
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        desenhaObjetoTela(renderer, camera->viewMatrix, objeto, zoom);

        SDL_RenderPresent(renderer);
    }

fim:
    desalocaObjeto(objeto);
    desalocaCamera(camera);
    desalocaTela(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
