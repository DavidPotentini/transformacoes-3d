#include <stdio.h>
#include <stdlib.h>
#include "algebra.h"

//Ajusta valores de um matriz 4d para uma matriz identidade
void criaIdentidade4d(float **novaMatriz){
    int i, j;
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            novaMatriz[i][j] = (i == j) ? 1.0f : 0.0f;
}

//Imprime uma matriz 4d no terminal
void imprimeMatriz4dDBG(float **matriz){
    int i, j;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++)
            printf("%9.3f ", matriz[i][j]);
        printf("\n");
    }
    printf("\n");
}

//Multiplica uma matriz 4d por um ponto (vetor homogeneo de 4 posicoes)
//Retorna um novo vetor de 4 posicoes alocado dinamicamente
float *multMatriz4dPonto(float **matriz, float *ponto){
    float *resultado = (float *) malloc(4 * sizeof(float));
    int i, j;
    for(i = 0; i < 4; i++){
        resultado[i] = 0.0f;
        for(j = 0; j < 4; j++)
            resultado[i] += matriz[i][j] * ponto[j];
    }
    return resultado;
}

//Multiplica duas matrizes 4d salvando o resultado na segunda matriz
//Calcula matrizB = matrizA * matrizB
void multMatriz4d(float **matrizA, float **matrizB){
    float temp[4][4];
    int i, j, k;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            temp[i][j] = 0.0f;
            for(k = 0; k < 4; k++)
                temp[i][j] += matrizA[i][k] * matrizB[k][j];
        }
    }
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            matrizB[i][j] = temp[i][j];
}
