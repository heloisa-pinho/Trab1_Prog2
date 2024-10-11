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

// Função para abrir uma imagem e calcular o LBP
float *abre_imagem(const char *imagem, int salvar)
{
    // Extrai o nome do arquivo (sem o diretório) para garantir que o caminho do LBP esteja correto
    char nome_arquivo_base[512];
    strcpy(nome_arquivo_base, imagem);
    char *nome_arquivo = basename(nome_arquivo_base);

    // Cria o caminho do arquivo LBP correspondente à imagem no diretório vetores_lbp/
    char nome_arquivo_lbp[512];
    snprintf(nome_arquivo_lbp, sizeof(nome_arquivo_lbp), "vetores_lbp/%s", nome_arquivo);

    float *vetor_pixel_lbp = carregar_lbp(nome_arquivo_lbp, 256);
    if (vetor_pixel_lbp != NULL)
    {
        return vetor_pixel_lbp;
    }
    free(vetor_pixel_lbp);

    // Abre a imagem
    FILE *inputFile = fopen(imagem, "rb");
    if (inputFile == NULL)
    {
        return 0;
    }

    char image_type[3];
    int width, height;
    int max_value;

    // Lê o tipo da imagem (P2 ou P5)
    fscanf(inputFile, "%s", image_type);

    // Verifica se é P2 ou P5
    if (strcmp(image_type, "P2") != 0 && strcmp(image_type, "P5") != 0)
    {
        fclose(inputFile);
        return 0;
    }

    fscanf(inputFile, "%d %d", &width, &height);
    fscanf(inputFile, "%d", &max_value);

    unsigned char *image_data = (unsigned char *)malloc(width * height * sizeof(unsigned char));
    memset(image_data, 0, width * height);
    fgetc(inputFile); // Avança o cursor no arquivo

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

    unsigned char *image_datalbp = (unsigned char *)malloc(new_width * new_height * sizeof(unsigned char));
    if (image_datalbp == NULL)
    {
        free(image_data);
        return 0;
    }

    // Calcular o LBP para cada pixel (ignorando as bordas)
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            // Calcular o LBP e salvar no novo array, ajustando o índice para a nova imagem
            image_datalbp[(y - 1) * new_width + (x - 1)] = calculaLBP(image_data, width, x, y);
        }
    }

    float *vetor_pixel = (float *)malloc((max_value + 1) * sizeof(float));
    cria_vetor_pixel(image_datalbp, vetor_pixel, width - 2, height - 2, max_value);

    // Salva o vetor LBP da imagem, somente os que estão na base
    if (salvar)
    {
        salvar_lbp(vetor_pixel, max_value, imagem);
    }

    free(image_data);
    free(image_datalbp);

    return vetor_pixel;
}

// Função para processar a imagem de teste e comparar com as imagens base
void compara_imagem_teste(const char *imagem_teste, int max_value)
{
    // nao salva na funçao abrir imagem o vetor lbp
    int salvar = 0;

    // Cria uma cópia da string para manipulação sem alterar o original
    char imagem_copia[512];
    strcpy(imagem_copia, imagem_teste);

    // Retira o .pgm da imagem na cópia
    char *extensao = strrchr(imagem_copia, '.');
    if (extensao != NULL)
    {
        *extensao = '\0';
    }
    char nome_arquivo_lbp[512];
    snprintf(nome_arquivo_lbp, sizeof(nome_arquivo_lbp), "vetores_lbp/%.495s.lbp", imagem_copia);

    // confere se a imagem teste ja nao esta na base
    FILE *arquivo_lbp = fopen(nome_arquivo_lbp, "rb");
    if (arquivo_lbp != NULL)
    {
        printf("Imagem mais similar: %s 0.000000\n", imagem_teste);
        fclose(arquivo_lbp);
        return;
    }

    float *vetor_pixel_teste = abre_imagem((char *)imagem_teste, salvar);
    if (vetor_pixel_teste == NULL)
    {
        return;
    }

    // Compara com as imagens do diretório base
    DIR *dirstream;
    struct dirent *direntry;

    dirstream = opendir("vetores_lbp");
    if (!dirstream)
    {
        free(vetor_pixel_teste);
        exit(1);
    }

    float menor_distancia = FLT_MAX;
    char imagem_mais_parecida[256];

    while ((direntry = readdir(dirstream)) != NULL)
    {
        // Tenta carregar o vetor LBP salvo da imagem base
        char nome_arquivo[512];
        snprintf(nome_arquivo, sizeof(nome_arquivo), "vetores_lbp/%s", direntry->d_name);

        // Ignorar "." e ".."
        if (strcmp(direntry->d_name, ".") == 0 || strcmp(direntry->d_name, "..") == 0)
        {
            continue;
        }

        float *vetor_base = carregar_lbp(nome_arquivo, max_value);
        if (vetor_base == NULL)
        {
            continue;
        }

        float distancia = distancia_euclidiana(vetor_pixel_teste, vetor_base, max_value);

        if (distancia < menor_distancia)
        {
            menor_distancia = distancia;
            strcpy(imagem_mais_parecida, direntry->d_name);
        }

        free(vetor_base);
    }

    // Salvar o vetor LBP da imagem de teste após a comparação, se nao houver uma img igual na base
    if (menor_distancia != 0)
        salvar_lbp(vetor_pixel_teste, max_value, imagem_teste);

    closedir(dirstream);

    printf("Imagem mais similar: %s %.6f\n", imagem_mais_parecida, menor_distancia);

    free(vetor_pixel_teste);
}

// Função para processar todas as imagens do diretório base
void abre_diretorio(const char *diretorio_base)
{
    DIR *dirstream;
    struct dirent *direntry;

    dirstream = opendir(diretorio_base);
    if (!dirstream)
    {
        exit(1);
    }

    while ((direntry = readdir(dirstream)) != NULL)
    {
        if (strcmp(direntry->d_name, ".") == 0 || strcmp(direntry->d_name, "..") == 0)
            continue;

        char caminho_completo[512];
        memset(caminho_completo, 0, sizeof(caminho_completo));

        // Construir o caminho completo da imagem
        snprintf(caminho_completo, sizeof(caminho_completo), "%s/%s", diretorio_base, direntry->d_name);

        int salvar = 1;
        float *vetor_pixel = abre_imagem(caminho_completo, salvar);

        free(vetor_pixel);
    }

    closedir(dirstream);
}