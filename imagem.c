#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <float.h>
#include <libgen.h>
#include "lbp.h"

// Função para calcular o LBP de uma imagem
unsigned char calculaLBP(unsigned char *imagem, int width, int x, int y)
{
    // calcula os pixels viznhos do pixel central
    int offsets[8][2] = {{x - 1, y - 1}, {x, y - 1}, {x + 1, y - 1}, {x - 1, y}, {x + 1, y}, {x - 1, y + 1}, {x, y + 1}, {x + 1, y + 1}};
    unsigned char pixel_C = imagem[y * width + x];
    unsigned char valor_Lbp = 0;

    for (int i = 0; i < 8; i++)
    {
        int vizinho_X = offsets[i][0];
        int vizinho_Y = offsets[i][1];

        if (imagem[vizinho_Y * width + vizinho_X] >= pixel_C)
        {
            valor_Lbp += (1 << i);
        }
    }

    return valor_Lbp;
}

// Função para criar o vetor de pixels
void cria_vetor_pixel(unsigned char *imagem, float *vetor_pixel, int width, int height, int max_value)
{

    for (int i = 0; i <= max_value; i++)
    {
        vetor_pixel[i] = 0.0f;
    }

    // Conta a frequência dos pixels
    for (int i = 0; i < (width * height); i++)
    {
        vetor_pixel[imagem[i]] += 1.0f;
    }

    // Normaliza os valores
    for (int i = 0; i <= max_value; i++)
    {
        vetor_pixel[i] = vetor_pixel[i] / (width * height);
    }
}

// Função para calcular a distância euclidiana entre dois vetores
float distancia_euclidiana(float *vetor1, float *vetor2, int tamanho)
{
    float soma = 0.0f;
    for (int i = 0; i <= tamanho; i++)
    {
        soma += powf(vetor1[i] - vetor2[i], 2);
    }
    return sqrtf(soma);
}

// Função para salvar o vetor LBP em um arquivo binário no diretorio vetores_lbp
void salvar_lbp(float *vetor_lbp, int max_value, const char *imagem)
{
    // Diretório fixo para salvar os vetores
    const char *diretorio_vetores = "vetores_lbp";

    // Verifica se o diretório já existe
    struct stat st = {0};
    if (stat(diretorio_vetores, &st) == -1)
    {
        if (mkdir(diretorio_vetores, 0700) != 0)
        {
            return;
        }
    }

    // Extrair apenas o nome do arquivo da imagem, sem o caminho
    const char *nome_imagem = strrchr(imagem, '/');
    if (nome_imagem == NULL)
    {
        nome_imagem = imagem;
    }
    else
    {
        nome_imagem++;
    }

    // Criar o nome do arquivo completo com o caminho do diretório
    char nome_arquivo[512];
    snprintf(nome_arquivo, sizeof(nome_arquivo), "%s/%.*s.lbp",
             diretorio_vetores,
             (int)(strrchr(nome_imagem, '.') - nome_imagem),
             nome_imagem);

    int tamanho_lbp = max_value + 1;

    FILE *fp = fopen(nome_arquivo, "wb");
    if (fp == NULL)
    {
        return;
    }

    // Escrever os dados do vetor LBP no arquivo
    fwrite(vetor_lbp, sizeof(float), tamanho_lbp, fp);

    // Fechar o arquivo
    fclose(fp);
}

// Função para carregar o vetor LBP de um arquivo binário
float *carregar_lbp(const char *imagem, int max_value)
{
    FILE *fp = fopen(imagem, "rb");
    if (fp == NULL)
    {
        return NULL;
    }

    float *vetor_lbp = (float *)malloc((max_value + 1) * sizeof(float));
    if (vetor_lbp == NULL)
    {
        fclose(fp);
        return NULL;
    }
    fread(vetor_lbp, sizeof(float), max_value + 1, fp);
    fclose(fp);

    return vetor_lbp;
}

// funcao para gerar igame lbp
void gerar_imagem_lbp(const char *imagem_entrada, const char *imagem_saida)
{
    // Abre a imagem de entrada
    FILE *inputFile = fopen(imagem_entrada, "rb");
    if (inputFile == NULL)
    {
        return;
    }

    char image_type[3];
    int width, height;
    int max_value;

    // Lê o cabeçalho da imagem (P2 ou P5)
    fscanf(inputFile, "%s", image_type);
    if (strcmp(image_type, "P2") != 0 && strcmp(image_type, "P5") != 0)
    {
        fclose(inputFile);
        return;
    }

    fscanf(inputFile, "%d %d", &width, &height);
    fscanf(inputFile, "%d", &max_value);

    unsigned char *image_data = (unsigned char *)malloc(width * height * sizeof(unsigned char));
    if (image_data == NULL)
    {
        fclose(inputFile);
        return;
    }

    fgetc(inputFile); // Avança o cursor para o próximo byte
    if (strcmp(image_type, "P2") == 0)
    {
        for (int i = 0; i < width * height; i++)
        {
            int pixel;
            fscanf(inputFile, "%d", &pixel);
            image_data[i] = (unsigned char)pixel;
        }
    }
    else if (strcmp(image_type, "P5") == 0)
    {
        fread(image_data, sizeof(unsigned char), width * height, inputFile);
    }
    fclose(inputFile);

    // Ajustar as dimensões da imagem LBP (sem bordas)
    int new_width = width - 2;
    int new_height = height - 2;

    unsigned char *image_lbp = (unsigned char *)malloc(new_width * new_height * sizeof(unsigned char));
    if (image_lbp == NULL)
    {
        free(image_data);
        return;
    }

    // Calcular o LBP para cada pixel (ignorando as bordas)
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            // Calcular o LBP e salvar no novo array, ajustando o índice para a nova imagem
            image_lbp[(y - 1) * new_width + (x - 1)] = calculaLBP(image_data, width, x, y);
        }
    }

    FILE *outputFile = fopen(imagem_saida, "wb");
    if (outputFile == NULL)
    {
        free(image_data);
        free(image_lbp);
        return;
    }

    // Salvar a imagem LBP no formato P5 (binário) ou P2 (ASCII)
    if (strcmp(image_type, "P2") == 0)
    {
        fprintf(outputFile, "P2\n%d %d\n%d\n", new_width, new_height, max_value);
        for (int i = 0; i < new_width * new_height; i++)
        {
            fprintf(outputFile, "%d ", image_lbp[i]);
            if ((i + 1) % new_width == 0)
            {
                fprintf(outputFile, "\n");
            }
        }
    }
    else if (strcmp(image_type, "P5") == 0)
    {
        fprintf(outputFile, "P5\n%d %d\n%d\n", new_width, new_height, max_value);
        fwrite(image_lbp, sizeof(unsigned char), new_width * new_height, outputFile);
    }

    fclose(outputFile);

    free(image_data);
    free(image_lbp);
}