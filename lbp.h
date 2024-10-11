#ifndef LBP_H
#define LBP_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <float.h>

// Função para calcular o LBP de uma imagem
unsigned char calculaLBP(unsigned char *image, int width, int x, int y);

// Função para criar o vetor de pixels
void cria_vetor_pixel(unsigned char *image, float *vetor_pixel, int width, int height, int max_value);

// Função para calcular a distância euclidiana entre dois vetores
float distancia_euclidiana(float *vetor1, float *vetor2, int tamanho);

// Função para salvar o vetor LBP em um arquivo binário
void salvar_lbp(float *vetor_lbp, int max_value, const char *imagem);

void gerar_imagem_lbp(const char *imagem_entrada, const char *imagem_saida);

// Função para carregar o vetor LBP de um arquivo binário
float *carregar_lbp(const char *imagem, int max_value);

// Função para abrir uma imagem e calcular o LBP
float *abre_imagem(const char *imagem, int salvar);

// Função para processar a imagem de teste e comparar com as imagens base
void compara_imagem_teste(const char *imagem_teste, int max_value);

// Função para processar todas as imagens do diretório base
void abre_diretorio(const char *diretorio_base);

#endif // LBP_H
