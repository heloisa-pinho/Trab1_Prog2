#include <stdio.h>
#include <stdlib.h>
#include "lbp.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    int opt;
    const char *diretorio_base = NULL;
    const char *imagem_teste = NULL;
    const char *imagem_saida = NULL;
    int max_value = 255;

    // Processa os argumentos da linha de comando
    while ((opt = getopt(argc, argv, "i:d:o:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            imagem_teste = optarg; // Recebe o nome da imagem de entrada para gerar o LBP
            break;
        case 'd':
            diretorio_base = optarg; // Recebe o diretório base para comparação
            break;
        case 'o':
            imagem_saida = optarg; // Recebe o nome da imagem de saída
            break;
        default:
            return 1;
        }
    }

    // Verifica se as opções foram fornecidas corretamente
    if (diretorio_base && imagem_teste)
    {
        abre_diretorio(diretorio_base);
        compara_imagem_teste(imagem_teste, max_value);
    }
    else if (imagem_teste && imagem_saida)
    {
        gerar_imagem_lbp(imagem_teste, imagem_saida);
    }
    else
    {
        return 1;
    }

    return 0;
}
