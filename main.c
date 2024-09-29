#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned char calculaLBP(unsigned char *image, int width, int height, int x, int y)
{
    int offsets[8][2] = {{x - 1, y - 1}, {x, y - 1}, {x + 1, y - 1}, {x - 1, y}, {x + 1, y}, {x - 1, y + 1}, {x, y + 1}, {x + 1, y + 1}};
    unsigned char center = image[y * width + x];
    unsigned char lbpValue = 0;

    for (int i = 0; i < 8; i++)
    {
        int neighborX = offsets[i][0];
        int neighborY = offsets[i][1];

        if (image[neighborY * width + neighborX] >= center)
            lbpValue = lbpValue + (1 << i);
    }

    return lbpValue;
}

int main()
{
    FILE *inputFile = fopen("Apuleia5.pgm", "rb");
    FILE *outputFile = fopen("output.pgm", "wb");
    FILE *lbpOutputFile = fopen("output_lbp2p2.pgm", "wb");

    if (inputFile == NULL || outputFile == NULL)
    {
        printf("Falha ao abrir os arquivos.\n");
        return 1;
    }

    char image_type[3];
    int width, height, max_value;

    fscanf(inputFile, "%s", image_type);
    fscanf(inputFile, "%d %d", &width, &height);
    fscanf(inputFile, "%d", &max_value);

    unsigned char *image_data = (unsigned char *)malloc(width * height * sizeof(unsigned char));
    unsigned char *image_datalbp = (unsigned char *)malloc((width) * (height) * sizeof(unsigned char));
    int total_pixels = width * height;
    int i;

    fprintf(outputFile, "%s\n%d %d\n%d\n", image_type, width, height, max_value);
    fprintf(lbpOutputFile, "%s\n%d %d\n%d\n", image_type, (width), (height), max_value);

    fgetc(inputFile);

    for (i = 0; i < total_pixels; i++)
    {
        fread(&image_data[i], sizeof(unsigned char), 1, inputFile);
    }

    fprintf(outputFile, "%s", image_data);

    int x, y;

    for (y = 1; y < height; y++)
    {
        for (x = 1; x < width; x++)
        {
            image_datalbp[y * width + x] = calculaLBP(image_data, width, height, x, y);
        }
    }

    fwrite(image_datalbp, sizeof(unsigned char), (width) * (height), lbpOutputFile);

    free(image_datalbp);
    free(image_data);
    fclose(inputFile);
    fclose(outputFile);
    fclose(lbpOutputFile);

    printf("Imagem copiada sem branco e LBP geradas com sucesso.\n");

    return 0;
}