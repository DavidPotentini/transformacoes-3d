#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "objeto.h"
#include "algebra.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//Aloca uma matriz 4x4 dinamicamente
static float **alocaMatriz4d(){
    int i;
    float **m = (float **) malloc(4 * sizeof(float *));
    for(i = 0; i < 4; i++)
        m[i] = (float *) malloc(4 * sizeof(float));
    return m;
}

//Desaloca uma matriz 4x4
static void desalocaMatriz4d(float **m){
    int i;
    for(i = 0; i < 4; i++)
        free(m[i]);
    free(m);
}

//Le as informacoes de um arquivo e as carrega num novo objeto alocado
tObjeto3d *carregaObjeto(char *nomeArquivo){
    FILE *arquivo = fopen(nomeArquivo, "r");
    if(arquivo == NULL){
        printf("Erro ao abrir o arquivo \"%s\"\n", nomeArquivo);
        return NULL;
    }

    tObjeto3d *objeto = (tObjeto3d *) malloc(sizeof(tObjeto3d));
    int i;

    //Le os vertices
    fscanf(arquivo, "%d", &objeto->nPontos);
    objeto->pontos = (float **) malloc(objeto->nPontos * sizeof(float *));
    for(i = 0; i < objeto->nPontos; i++){
        //Cada ponto e guardado como vetor homogeneo [x, y, z, 1]
        objeto->pontos[i] = (float *) malloc(4 * sizeof(float));
        fscanf(arquivo, "%f %f %f", &objeto->pontos[i][0], &objeto->pontos[i][1], &objeto->pontos[i][2]);
        objeto->pontos[i][3] = 1.0f;
    }

    //Le as arestas
    fscanf(arquivo, "%d", &objeto->nArestas);
    objeto->arestas = (int **) malloc(objeto->nArestas * sizeof(int *));
    for(i = 0; i < objeto->nArestas; i++){
        objeto->arestas[i] = (int *) malloc(2 * sizeof(int));
        fscanf(arquivo, "%d %d", &objeto->arestas[i][0], &objeto->arestas[i][1]);
    }

    fclose(arquivo);

    //Inicializa os parametros de transformacao acumulados
    objeto->transl = (float *) malloc(3 * sizeof(float));
    objeto->escala = (float *) malloc(3 * sizeof(float));
    objeto->transl[0] = objeto->transl[1] = objeto->transl[2] = 0.0f;
    objeto->escala[0] = objeto->escala[1] = objeto->escala[2] = 1.0f;
    objeto->Rx = objeto->Ry = objeto->Rz = 0.0f;

    //modelMatrix comeca como identidade
    objeto->modelMatrix = alocaMatriz4d();
    criaIdentidade4d(objeto->modelMatrix);

    return objeto;
}

//Altera a modelMatrix de um objeto para redimenciona-lo segundo os parametros escalaX, escalaY e escalaZ
void escalaObjeto(tObjeto3d *objeto, float escalaX, float escalaY, float escalaZ){
    float **m = alocaMatriz4d();
    criaIdentidade4d(m);
    m[0][0] = escalaX;
    m[1][1] = escalaY;
    m[2][2] = escalaZ;

    //modelMatrix = escala * modelMatrix
    multMatriz4d(m, objeto->modelMatrix);
    desalocaMatriz4d(m);

    objeto->escala[0] *= escalaX;
    objeto->escala[1] *= escalaY;
    objeto->escala[2] *= escalaZ;
}

//Altera a modelMatrix de um objeto para translada-lo segundo os parametros transX, transY e transZ
void transladaObjeto(tObjeto3d *objeto, float transX, float transY, float transZ){
    float **m = alocaMatriz4d();
    criaIdentidade4d(m);
    m[0][3] = transX;
    m[1][3] = transY;
    m[2][3] = transZ;

    //modelMatrix = translacao * modelMatrix
    multMatriz4d(m, objeto->modelMatrix);
    desalocaMatriz4d(m);

    objeto->transl[0] += transX;
    objeto->transl[1] += transY;
    objeto->transl[2] += transZ;
}

//Altera a modelMatrix de um objeto para rotaciona-lo ao redor do eixo X segundo o angulo informado (em graus)
void rotacionaObjetoEixoX(tObjeto3d *objeto, float angulo){
    float rad = angulo * (float) M_PI / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    float **m = alocaMatriz4d();
    criaIdentidade4d(m);
    m[1][1] =  c; m[1][2] = -s;
    m[2][1] =  s; m[2][2] =  c;

    multMatriz4d(m, objeto->modelMatrix);
    desalocaMatriz4d(m);

    objeto->Rx += angulo;
}

//Altera a modelMatrix de um objeto para rotaciona-lo ao redor do eixo Y segundo o angulo informado (em graus)
void rotacionaObjetoEixoY(tObjeto3d *objeto, float angulo){
    float rad = angulo * (float) M_PI / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    float **m = alocaMatriz4d();
    criaIdentidade4d(m);
    m[0][0] =  c; m[0][2] =  s;
    m[2][0] = -s; m[2][2] =  c;

    multMatriz4d(m, objeto->modelMatrix);
    desalocaMatriz4d(m);

    objeto->Ry += angulo;
}

//Altera a modelMatrix de um objeto para rotaciona-lo ao redor do eixo Z segundo o angulo informado (em graus)
void rotacionaObjetoEixoZ(tObjeto3d *objeto, float angulo){
    float rad = angulo * (float) M_PI / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    float **m = alocaMatriz4d();
    criaIdentidade4d(m);
    m[0][0] =  c; m[0][1] = -s;
    m[1][0] =  s; m[1][1] =  c;

    multMatriz4d(m, objeto->modelMatrix);
    desalocaMatriz4d(m);

    objeto->Rz += angulo;
}

//Imprime um objeto no terminal
void imprimeObjetoDBG(tObjeto3d *objeto){
    int i;
    printf("=== Objeto 3D ===\n");
    printf("nPontos: %d\n", objeto->nPontos);
    for(i = 0; i < objeto->nPontos; i++)
        printf("  v%d: (%.2f, %.2f, %.2f)\n", i, objeto->pontos[i][0], objeto->pontos[i][1], objeto->pontos[i][2]);

    printf("nArestas: %d\n", objeto->nArestas);
    for(i = 0; i < objeto->nArestas; i++)
        printf("  a%d: %d -> %d\n", i, objeto->arestas[i][0], objeto->arestas[i][1]);

    printf("transl: (%.2f, %.2f, %.2f)\n", objeto->transl[0], objeto->transl[1], objeto->transl[2]);
    printf("escala: (%.2f, %.2f, %.2f)\n", objeto->escala[0], objeto->escala[1], objeto->escala[2]);
    printf("rotacao: Rx=%.2f Ry=%.2f Rz=%.2f\n", objeto->Rx, objeto->Ry, objeto->Rz);
    printf("modelMatrix:\n");
    imprimeMatriz4dDBG(objeto->modelMatrix);
}

//Desaloca o objeto
void desalocaObjeto(tObjeto3d *objeto){
    int i;
    if(objeto == NULL) return;

    for(i = 0; i < objeto->nPontos; i++)
        free(objeto->pontos[i]);
    free(objeto->pontos);

    for(i = 0; i < objeto->nArestas; i++)
        free(objeto->arestas[i]);
    free(objeto->arestas);

    free(objeto->transl);
    free(objeto->escala);
    desalocaMatriz4d(objeto->modelMatrix);

    free(objeto);
}
