#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DATASET_SIZE 100
#define N_OF_FEATURES 536

void checkArgs(char const *argv[]);
double** loadDatasetFile(); 
void sortIndexes(int *trainingIndexes, int *testingIndexes);
int existsIn(int number, int *array, int length);

int main(int argc, char const *argv[])
{
    int trainingIndexes[50], testingIndexes[50];
    double **dataset;

    srand(time(NULL)); // Seed rand funcion with time

    checkArgs(argv); // Check if argument is valid
    dataset = loadDatasetFile(); // Load dataset from file
    sortIndexes(trainingIndexes, testingIndexes); // Sort dataset indexes randomly and mount arrays to train and test network
        
    for(int i = 0; i < DATASET_SIZE; i++)
        free(dataset[i]);
    free(dataset);
    
    return 0;
}

void checkArgs(char const *argv[])
{
    if(argv[1] == NULL)
    {
        printf("Argument not identified. Exiting program...\n");
        exit(-11);
    }
    else if(atoi(argv[1]) <= 0)
    {
        printf("Given argument must be an integer greater than 0.\n");
        exit(-12);
    }
}

int existsIn(int number, int *array, int length){
    int exists = 0;
    for(int i = 0; i < length; i++){
        if(array[i] == number)
            return exists = 1;
    }    
    return exists;
}

double** loadDatasetFile()
{
    FILE *file;
    double **dataset;
    int nOfReads = 0; // Flag to check if file is empty

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
        nOfReads++;

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

    if(nOfReads != 53700){printf("Features file is damaged, run \"./calcFeatures\" to calculate features from dataset images\n");exit(-100);}
    
    fclose(file);
    return dataset;
}

void shuffle( int *array, int n ) 
{ 
    for (int i = n-1; i > 0; i--) 
    { 
        // Pick a random index from 0 to i 
        int j = rand() % (i+1); 
  
        // Swap array index with the element at random index 
        int temp = array[i]; 
        array[i] = array[j]; 
        array[j] = temp; 
    } 
} 

void sortIndexes(int *trainingIndexes, int *testingIndexes){
    int i = 0, j = 0, n = 0;
    
    while(i < 25) { // Generates 25 grass indexes to use in training set
        int index = rand() % 50;
        if(!existsIn(index, trainingIndexes, 50)) // Check if index is already in the array
        {
            trainingIndexes[i] = index;
            i++;
        }
    }

    i=0;
    j=0;
    while(i < 25) { // Generates 25 asphalt indexes to use in training set
        int index = rand() % (99 + 1 - 50) + 50;
        if(!existsIn(index, trainingIndexes, 50)) // Check if index is already in the array
        {
            trainingIndexes[i + 25] = index;
            i++;
        }
    }
        
    while(j < 50) { // Increments n until find numbers that aren't in array
        if(!existsIn(n, trainingIndexes, 50)){
            testingIndexes[j] = n;
            j++;
        }
        n++;
    }

    // Shuffle arrays to not bias results based on order of insertion in array
    shuffle(trainingIndexes, 50);
    shuffle(testingIndexes, 50);
}