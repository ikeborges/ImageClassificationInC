#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

int existe_em(int numero, int *vetor, int tamanho_vetor);
int **receber_imagem(int numero_imagem, char classe[], int *tamanho_imagem);
double *calcular_ilbp(int **imagem, int *tamanho_imagem);
int calcular_decimal(int *b);
int menor_binario(int *b);
double menor_valor(double *vetor, int tam);
double maior_valor(double *vetor, int tam);
double *concatenar(double *vetor1, double *vetor2);
double *normaliza_vetor(double *vetor, int tam);
double *calcular_glcm(int **imagem, int *tamanho_imagem);
double calcular_contraste(int **matriz);
double calcular_energia(int **matriz);
double calcular_homogeneidade(int **matriz);

int main(int argc, char const *argv[])
{
    int **imagem, tamanho_imagem;
    double *ilbp, *glcm, *descritor_normalizado, *descritor_imagem;

    FILE *arquivo;

    arquivo = fopen("./features.txt", "w+");
    if(arquivo == NULL){printf("Erro ao abrir ou criar arquivo!\n");exit(-8);}

    printf("\n\nCalculando features da classe GRAMA\n\n");

    for(int i = 1; i <= 50; i++){
        // Recebe imagem
        imagem = receber_imagem(i, "grass", &tamanho_imagem);
        printf("Imagem %d recebida\n", i);

        // Calcula métricas
        ilbp = calcular_ilbp(imagem, &tamanho_imagem);
        glcm = calcular_glcm(imagem, &tamanho_imagem);

        // Concatena métricas e normaliza vetor
        descritor_imagem = concatenar(ilbp, glcm);
        descritor_normalizado = normaliza_vetor(descritor_imagem, 536);

        // Escreve as métricas no arquivo de features
        for(int j=0;j<536;j++)
            fprintf(arquivo, "%lf ", descritor_normalizado[j]);
        
        // Insere 1 ao final de cada linha para marcar o vetor como sendo grama
        fprintf(arquivo, "1\n");

        printf("Imagem %d calculada\n", i);
        printf("------------\n");

        for(int j = 0; j < tamanho_imagem; j++)
            free(imagem[j]);
        free(imagem);
        free(descritor_imagem);
        free(descritor_normalizado);
        free(glcm);
    }

    printf("\n\nCalculando features da classe ASFALTO\n\n");

    for(int i = 1; i <= 50; i++){
        // Recebe imagem
        imagem = receber_imagem(i, "grass", &tamanho_imagem);
        printf("Imagem %d recebida\n", i);

        // Calcula métricas
        ilbp = calcular_ilbp(imagem, &tamanho_imagem);
        glcm = calcular_glcm(imagem, &tamanho_imagem);

        // Concatena métricas e normaliza vetor
        descritor_imagem = concatenar(ilbp, glcm);
        descritor_normalizado = normaliza_vetor(descritor_imagem, 536);

        // Escreve as métricas no arquivo de features
        for(int j=0;j<536;j++)
            fprintf(arquivo, "%lf ", descritor_normalizado[j]);
        
        // Insere 0 ao final de cada linha para marcar o vetor como sendo asfalto
        fprintf(arquivo, "0\n");

        printf("Imagem %d calculada\n", i);
        printf("------------\n");

        for(int j = 0; j < tamanho_imagem; j++)
            free(imagem[j]);
        free(imagem);
        free(descritor_imagem);
        free(descritor_normalizado);
        free(glcm);
    }
    
    fclose(arquivo);

    return 0;
}

int existe_em(int numero, int *vetor, int tamanho_vetor){
    int existe = 0;
    for(int i = 0; i < tamanho_vetor; i++){
        if(*(vetor + i) == numero)
            return existe = 1;
    }
    
    return existe;
}

int **receber_imagem(int numero_imagem, char classe[], int *tamanho_imagem){
    int **imagem, numero, linha, coluna;
    char nome_arquivo[50], caractere;
    sprintf(nome_arquivo, "%s%s%c%s%c%.2d%s", "DataSet/", classe, '/', classe, '_', numero_imagem, ".txt");

    FILE *fp;

    fp = fopen(nome_arquivo, "r");
    if(fp == NULL) {
        printf("Erro ao abrir arquivo!\n");
        exit(1);
    }

    imagem = (int**)malloc(sizeof(int*));
    if(imagem == NULL) {
        printf("Erro ao alocar memória para linha 0\n");
        exit(1);
    }

    imagem[0] = (int*)malloc(sizeof(int));
    if(imagem[0] == NULL){
        printf("Erro ao alocar memório para coluna 0\n");
        free(imagem);
        exit(1);
    }

    linha = 0;
    coluna = 0;

    while(!feof(fp)){
        fscanf(fp, "%d%c", &numero, &caractere);
        imagem[linha][coluna] = numero;

        if(caractere == '\n'){
            linha++;
            imagem = (int**)realloc(imagem, (linha + 1)*sizeof(int*));

            if(imagem == NULL){
                printf("Erro ao alocar memória para linha %d\n", linha + 1);
                for(int i = 0; i < linha; i++)
                    free(imagem[i]);                
                free(imagem);
                exit(1);
            }

            imagem[linha] = (int*)malloc(sizeof(int));
            
            if(imagem[linha] == NULL){
                printf("Erro ao alocar memória para coluna %d\n", coluna + 1);
                for(int i = 0; i < linha; i++)
                    free(imagem[i]);                
                free(imagem);
                exit(1);
            }
            coluna = 0;
            continue;
        }

        coluna++;
        imagem[linha] = (int*)realloc(imagem[linha], (coluna + 1)*sizeof(int));

        if(imagem[linha] == NULL){
            printf("Erro ao alocar memória para coluna %d\n", coluna + 1);
            for(int i = 0; i < linha; i++)
                free(imagem[i]);                
            free(imagem);
            exit(1);
        }
    }
    
    fclose(fp);

    *tamanho_imagem = linha - 1;
    return imagem;
}

double *calcular_ilbp(int **imagem, int *tamanho_imagem){
    int p[9], b[9], ilbp;
    double media, *vetor_ilbp;

    vetor_ilbp = (double *)calloc(512, sizeof(double));
    if(vetor_ilbp == NULL){
        printf("Erro ao alocar memória para vetor ILBP");
        exit(1);
    }

    printf("Calculando ILBP...\n");

    for(int i = 1; i < *tamanho_imagem - 1; i++){
        for(int j = 1; j < *tamanho_imagem - 1; j++){
            p[0] = imagem[i-1][j-1];
            p[1] = imagem[i-1][j];
            p[2] = imagem[i-1][j+1];
            p[3] = imagem[i][j-1];
            p[4] = imagem[i][j];
            p[5] = imagem[i][j+1];
            p[6] = imagem[i+1][j-1];
            p[7] = imagem[i+1][j];
            p[8] = imagem[i+1][j+1];

            media = (p[0] + p[1] + p[2] + p[3] + p[4] + p[5] + p[6] + p[7] + p[8])/9.0;

            for(int k = 0; k < 9; k++){
                if(p[k] > media)
                    b[k] = 1;
                else
                    b[k] = 0;
            }

            ilbp = menor_binario(b);
            vetor_ilbp[ilbp] += 1;
        }
    }
    printf("ILBP calculado com sucesso!\n");
    return vetor_ilbp;
}

int calcular_decimal(int *b){
    int numero_decimal = 1*b[8] + 2*b[7] + 4*b[6] + 8*b[5] + 16*b[4] + 32*b[3] + 64*b[2] + 128*b[1] + 256*b[0];
    return numero_decimal;
}

int menor_binario(int *b){
    int decimal, menor_binario, aux0, aux1, aux2, aux3, aux4, aux5, aux6, aux7;

    decimal = calcular_decimal(b);
    menor_binario = decimal;
    
    for(int i = 0; i < 9; i++){
        aux0 = b[0];
        b[0] = b[8];
        aux1 = b[1];
        b[1] = aux0;
        aux2 = b[2];
        b[2] = aux1;
        aux3 = b[3];
        b[3] = aux2;
        aux4 = b[4];
        b[4] = aux3;
        aux5 = b[5];
        b[5] = aux4;
        aux6 = b[6];
        b[6] = aux5;
        aux7 = b[7];
        b[7] = aux6;
        b[8] = aux7;

        decimal = calcular_decimal(b);
        if(decimal < menor_binario)
            menor_binario = decimal;
    }

    return menor_binario;
}

double menor_valor(double *vetor, int tam){
    double menor = vetor[0];
    for(int i = 1; i < tam; i++){
        if(vetor[i] < menor)
            menor = vetor[i];
    }

    return menor;
}

double maior_valor(double *vetor, int tam){
    double maior = vetor[0];
    for(int i = 1; i < tam; i++){
        if(vetor[i] > maior)
            maior = vetor[i];
    }

    return maior;
}

double *concatenar(double *vetor1, double *vetor2){
    printf("Concatenando vetores...\n");

    double *vetor;

    vetor = vetor1;

    vetor = (double *)realloc(vetor, 536*sizeof(double));
    if(vetor == NULL){printf("Erro ao alocar descritor da imagem\n");exit(1);}

    for(int i = 0; i < 24; i++)
        vetor[512 + i] = vetor2[i];

    printf("Vetores concatenados com sucesso!\n");
    return vetor;
}

double *normaliza_vetor(double *vetor, int tam){
    printf("Normalizando vetor...\n");

    double *vetor_normalizado;
    double xmin = menor_valor(vetor, tam);
    double xmax = maior_valor(vetor, tam);

    vetor_normalizado = (double *)malloc(tam*sizeof(double));

    for(int i = 0; i < tam; i++)
        vetor_normalizado[i] = (vetor[i] - xmin)/(xmax - xmin);

    printf("Vetor normalizado com sucesso!\n");

    return vetor_normalizado;
}

double *calcular_glcm(int **imagem, int *tamanho_imagem){
    int **norte, **nordeste, **leste, **sudeste, **sul, **sudoeste, **oeste, **noroeste;
    double *resultado;

    printf("Calculando GLCM...\n");

    // Aloca memória para vetor resultado
    resultado = (double *)calloc(24, sizeof(double));
    if(resultado == NULL){printf("Erro ao alocar memória para vetor resultado");exit(1);}

    // Aloca memória para vetores glcm
    norte = (int **)calloc(256, sizeof(int *));
    if(norte == NULL){printf("Erro ao alocar memória para linhas vetor direção norte");exit(1);}
    for(int i = 0; i < 256; i++){
        norte[i] = (int *)calloc(256, sizeof(int));
        if(norte[i] == NULL){printf("Erro ao alocar memória para colunas vetor direção norte");free(norte);exit(1);}
    }

    nordeste = (int **)calloc(256, sizeof(int *));
    if(nordeste == NULL){printf("Erro ao alocar memória para linhas vetor direção nordeste");exit(1);}
    for(int i = 0; i < 256; i++){
        nordeste[i] = (int *)calloc(256, sizeof(int));
        if(nordeste[i] == NULL){printf("Erro ao alocar memória para colunas vetor direção nordeste");free(nordeste);exit(1);}
    }

    leste = (int **)calloc(256, sizeof(int *));
    if(leste == NULL){printf("Erro ao alocar memória para linhas vetor direção leste");exit(1);}
    for(int i = 0; i < 256; i++){
        leste[i] = (int *)calloc(256, sizeof(int));
        if(leste[i] == NULL){printf("Erro ao alocar memória para colunas vetor direção leste");free(leste);exit(1);}
    }

    sudeste = (int **)calloc(256, sizeof(int *));
    if(sudeste == NULL){printf("Erro ao alocar memória para linhas vetor direção sudeste");exit(1);}
    for(int i = 0; i < 256; i++){
        sudeste[i] = (int *)calloc(256, sizeof(int));
        if(sudeste[i] == NULL){printf("Erro ao alocar memória para colunas vetor direção sudeste");free(sudeste);exit(1);}
    }

    sul = (int **)calloc(256, sizeof(int *));
    if(sul == NULL){printf("Erro ao alocar memória para linhas vetor direção sul");exit(1);}
    for(int i = 0; i < 256; i++){
        sul[i] = (int *)calloc(256, sizeof(int));
        if(sul[i] == NULL){printf("Erro ao alocar memória para colunas vetor direção sul");free(sul);exit(1);}
    }

    sudoeste = (int **)calloc(256, sizeof(int *));
    if(sudoeste == NULL){printf("Erro ao alocar memória para linhas vetor direção sudoeste");exit(1);}
    for(int i = 0; i < 256; i++){
        sudoeste[i] = (int *)calloc(256, sizeof(int));
        if(sudoeste[i] == NULL){printf("Erro ao alocar memória para colunas vetor direção sudoeste");free(sudoeste);exit(1);}
    }

    oeste = (int **)calloc(256, sizeof(int *));
    if(oeste == NULL){printf("Erro ao alocar memória para linhas vetor direção oeste");exit(1);}
    for(int i = 0; i < 256; i++){
        oeste[i] = (int *)calloc(256, sizeof(int));
        if(oeste[i] == NULL){printf("Erro ao alocar memória para colunas vetor direção oeste");free(oeste);exit(1);}
    }

    noroeste = (int **)calloc(256, sizeof(int *));
    if(noroeste == NULL){printf("Erro ao alocar memória para linhas vetor direção noroeste");exit(1);}
    for(int i = 0; i < 256; i++){
        noroeste[i] = (int *)calloc(256, sizeof(int));
        if(noroeste[i] == NULL){printf("Erro ao alocar memória para colunas vetor direção noroeste");free(noroeste);exit(1);}
    }

    for(int i = 1; i < *tamanho_imagem - 1; i++){
        for(int j = 1; j < *tamanho_imagem - 1; j++){
            int ponto_central = imagem[i][j];

            // Norte
            int valor_norte = imagem[i-1][j];
            norte[ponto_central][valor_norte]++;

            // Nordeste
            int valor_nordeste = imagem[i-1][j+1];
            nordeste[ponto_central][valor_nordeste]++;

            // Leste
            int valor_leste = imagem[i][j+1];
            leste[ponto_central][valor_leste]++;

            // Sudeste
            int valor_sudeste = imagem[i+1][j+1];
            sudeste[ponto_central][valor_sudeste]++;

            // Sul
            int valor_sul = imagem[i+1][j];
            sul[ponto_central][valor_sul]++;

            // Sudoeste
            int valor_sudoeste = imagem[i+1][j-1];
            sudoeste[ponto_central][valor_sudoeste]++;

            // Oeste
            int valor_oeste = imagem[i][j-1];
            oeste[ponto_central][valor_oeste]++;

            // Noroeste
            int valor_noroeste = imagem[i-1][j-1];
            noroeste[ponto_central][valor_noroeste]++;
        }
    }

    resultado[0] = calcular_contraste(norte);
    resultado[1] = calcular_energia(norte);
    resultado[2] = calcular_homogeneidade(norte);

    resultado[3] = calcular_contraste(nordeste);
    resultado[4] = calcular_energia(nordeste);
    resultado[5] = calcular_homogeneidade(nordeste);

    resultado[6] = calcular_contraste(leste);
    resultado[7] = calcular_energia(leste);
    resultado[8] = calcular_homogeneidade(leste);

    resultado[9] = calcular_contraste(sudeste);
    resultado[10] = calcular_energia(sudeste);
    resultado[11] = calcular_homogeneidade(sudeste);

    resultado[12] = calcular_contraste(sul);
    resultado[13] = calcular_energia(sul);
    resultado[14] = calcular_homogeneidade(sul);

    resultado[15] = calcular_contraste(sudoeste);
    resultado[16] = calcular_energia(sudoeste);
    resultado[17] = calcular_homogeneidade(sudoeste);

    resultado[18] = calcular_contraste(oeste);
    resultado[19] = calcular_energia(oeste);
    resultado[20] = calcular_homogeneidade(oeste);

    resultado[21] = calcular_contraste(noroeste);
    resultado[22] = calcular_energia(noroeste);
    resultado[23] = calcular_homogeneidade(noroeste);

    for(int i = 0; i < 256; i++)
        free(norte[i]);
    free(norte);

    for(int i = 0; i < 256; i++)
        free(nordeste[i]);
    free(nordeste);

    for(int i = 0; i < 256; i++)
            free(leste[i]);
    free(leste);

    for(int i = 0; i < 256; i++)
        free(sudeste[i]);
    free(sudeste);

    for(int i = 0; i < 256; i++)
        free(sul[i]);
    free(sul);

    for(int i = 0; i < 256; i++)
        free(sudoeste[i]);
    free(sudoeste);

    for(int i = 0; i < 256; i++)
        free(oeste[i]);
    free(oeste);

    for(int i = 0; i < 256; i++)
        free(noroeste[i]);
    free(noroeste);

    printf("GLCM calculado com sucesso!\n");

    return resultado;
}

double calcular_contraste(int **matriz){
    double soma = 0;
    for(int i = 0; i < 256; i++){
        for(int j = 0; j < 256; j++)
            soma += pow(i - j, 2)*matriz[i][j];
    }

    return soma;
}

double calcular_energia(int **matriz){
    double soma = 0;
    for(int i = 0; i < 256; i++){
        for(int j = 0; j < 256; j++)
            soma += pow(matriz[i][j], 2);
    }

    return soma;
}

double calcular_homogeneidade(int **matriz){
    double soma = 0;
    for(int i = 0; i < 256; i++){
        for(int j = 0; j < 256; j++)
            soma += (matriz[i][j])/(1 + sqrt(pow(i-j, 2)));
    }

    return soma;
}
