#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "camera.h"
#include "algebra.h"

//Aloca uma matriz 4x4 dinamicamente
static float **alocaMatriz4d(){
    int i;
    float **m = (float **) malloc(4 * sizeof(float *));
    for(i = 0; i < 4; i++)
        m[i] = (float *) malloc(4 * sizeof(float));
    return m;
}

//Produto escalar entre dois vetores de 3 posicoes
static float dot3(float *a, float *b){
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

//Produto vetorial r = a x b (vetores de 3 posicoes)
static void cross3(float *a, float *b, float *r){
    r[0] = a[1]*b[2] - a[2]*b[1];
    r[1] = a[2]*b[0] - a[0]*b[2];
    r[2] = a[0]*b[1] - a[1]*b[0];
}

//Normaliza um vetor de 3 posicoes
static void normaliza3(float *v){
    float n = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if(n > 0.0f){
        v[0] /= n;
        v[1] /= n;
        v[2] /= n;
    }
}

//Aloca a estrutura de memoria para representar a camera
tCamera3d *criaCamera(){
    tCamera3d *camera = (tCamera3d *) malloc(sizeof(tCamera3d));
    camera->posicao = (float *) malloc(3 * sizeof(float));
    camera->foco    = (float *) malloc(3 * sizeof(float));
    camera->cima    = (float *) malloc(3 * sizeof(float));
    camera->viewMatrix = alocaMatriz4d();
    criaIdentidade4d(camera->viewMatrix);
    return camera;
}

//Recebe os parametros especificos da camera e calcula viewMatrix
void defineCamera(tCamera3d *camera, float posX, float posY, float posZ, float focX, float focY, float focZ, float cimX, float cimY, float cimZ){
    camera->posicao[0] = posX; camera->posicao[1] = posY; camera->posicao[2] = posZ;
    camera->foco[0]    = focX; camera->foco[1]    = focY; camera->foco[2]    = focZ;
    camera->cima[0]    = cimX; camera->cima[1]    = cimY; camera->cima[2]    = cimZ;

    //n = posicao - foco (aponta para tras da camera)
    float n[3];
    n[0] = posX - focX;
    n[1] = posY - focY;
    n[2] = posZ - focZ;
    normaliza3(n);

    //u = cima x n  (eixo lateral / direita)
    float u[3];
    cross3(camera->cima, n, u);
    normaliza3(u);

    //v = n x u  (eixo "para cima" verdadeiro)
    float v[3];
    cross3(n, u, v);

    //Monta a viewMatrix (rotacao pelos eixos da camera + translacao para a origem)
    float **m = camera->viewMatrix;
    m[0][0] = u[0]; m[0][1] = u[1]; m[0][2] = u[2]; m[0][3] = -dot3(u, camera->posicao);
    m[1][0] = v[0]; m[1][1] = v[1]; m[1][2] = v[2]; m[1][3] = -dot3(v, camera->posicao);
    m[2][0] = n[0]; m[2][1] = n[1]; m[2][2] = n[2]; m[2][3] = -dot3(n, camera->posicao);
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

//Desaloca a camera
void desalocaCamera(tCamera3d *camera){
    int i;
    if(camera == NULL) return;
    free(camera->posicao);
    free(camera->foco);
    free(camera->cima);
    for(i = 0; i < 4; i++)
        free(camera->viewMatrix[i]);
    free(camera->viewMatrix);
    free(camera);
}
