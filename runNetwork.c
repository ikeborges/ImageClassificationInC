#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DATASET_SIZE 100
#define N_OF_FEATURES 536

double** loadDatasetFile(); 

int main(int argc, char const *argv[])
{
    double **dataset; 

    dataset = loadDatasetFile();
    
    for(int i = 0; i < DATASET_SIZE; i++)
    {
        for(int j = 0; j < N_OF_FEATURES + 1; j++)
        {
            printf("%lf ", dataset[i][j]);
        }
        printf("\n---------------------\n");
        
    }
        
    for(int i = 0; i < DATASET_SIZE; i++)
        free(dataset[i]);
    free(dataset);
    

    return 0;
}

double** loadDatasetFile()
{
    FILE *file;
    double **dataset;

    file = fopen("./features.txt", "r");
    if(file == NULL){printf("Error while opening features file\n");exit(-1);}
    
    // Allocate dataset matrix
    dataset = (double **)malloc(sizeof(double *) * DATASET_SIZE);
    if(dataset == NULL){printf("Error while allocating memory for dataset\n");exit(-2);}

    // Allocate dataset vectors    
    for(int i = 0; i < DATASET_SIZE; i++)
        dataset[i] = (double *)malloc(sizeof(double) * (N_OF_FEATURES + 1));

    int i = 0, j = 0;
    double feature;
    char character;

    while(!feof(file))
    {
        fscanf(file, "%lf%c ", &feature, &character); // Get a number and a char
        dataset[i][j] = feature;

        // printf("%d %d\n", i, j);
        
        if(character == ' ') // If char is a whitespace, go to next position in vector
        {
            j++;
        }
        else // If char is new line, go to next vector on first position
        {
            i++;
            j=0;
        }

    }
    
    fclose(file);
    return dataset;
}