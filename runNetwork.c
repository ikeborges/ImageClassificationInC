#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DATASET_SIZE 100 // Size of dataset
#define N_OF_FEATURES 536 // Number of features each item in dataset has
#define GRASS 1 // Representation of Grass
#define ASPHALT 0 // Representation of Asphalt
#define RANDOM_MIN -16000.0 // Minimum number for random weight and bias generation
#define RANDOM_MAX 15999.9  // Maximum number for random weight and bias generation

/* 
    As C's structures don't have functions in their body the neurons here (aka struct NEURON) only stores weights and biases,
    leaving the storage of inputs and outputs as well as the activation function application to the propagation and backpropagation functions themselves
*/
typedef struct NEURON
{
    double *weights;
    double bias;
} Neuron;

void checkArgs(char const *argv[]);
Neuron **createAndInitNetwork(int inputSize, int hiddenLayerSize);
int existsIn(int number, int *array, int length);
double *feedLayer(Neuron *layer, double *input, int layerSize, int inputSize);
double feedNetwork(double *imageFeatures, Neuron **network, int inputSize, int hiddenLayerSize);
void freeResources(double **dataset, Neuron **network, int hiddenLayerSize);
double getRandomNumber();
Neuron *initLayer(Neuron *layer, int nOfWeights, int nOfNeurons);
double** loadDatasetFile(); 
void shuffle(int *array, int n);
void sortIndexes(int *trainingIndexes, int *testingIndexes);

int main(int argc, char const *argv[])
{
    int trainingIndexes[50] = {0}, testingIndexes[50] = {0};
    double **dataset, result;
    Neuron **network;
    srand(time(NULL)); // Seed rand funcion with time

    checkArgs(argv); // Check if argument is valid
    int hiddenLayerSize = atoi(argv[1]);

    dataset = loadDatasetFile(); // Load dataset from file
    sortIndexes(trainingIndexes, testingIndexes); // Sort dataset indexes randomly and mount arrays to train and test network
    network = createAndInitNetwork(N_OF_FEATURES, hiddenLayerSize); // Creates and initalizes network

    ///////////////////////////////////////////////////
    /*              Network training                */
    /////////////////////////////////////////////////

    result = feedNetwork(dataset[trainingIndexes[0]], network, N_OF_FEATURES, hiddenLayerSize);

    freeResources(dataset, network, hiddenLayerSize);
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

Neuron **createAndInitNetwork(int inputSize, int hiddenLayerSize)
{
    Neuron **network;

    network = (Neuron **)malloc(sizeof(Neuron *) * 3); // Allocate network with the 3 layers as arrays
    if(network==NULL){printf("Error while allocating memory for network");exit(-15);}

    // Allocate layers
    network[0] = (Neuron *)malloc(sizeof(Neuron) * inputSize); // Allocate INPUT layer
    network[1] = (Neuron *)malloc(sizeof(Neuron) * hiddenLayerSize); // Allocate HIDDEN layer
    network[2] = (Neuron *)malloc(sizeof(Neuron) * 1); // Allocate OUTPUT layer

    // Check if allocation worked
    if(network[0]==NULL){printf("Error while allocating memory for input layer");exit(-16);}
    if(network[1]==NULL){printf("Error while allocating memory for hidden layer");exit(-17);}
    if(network[2]==NULL){printf("Error while allocating memory for output layer");exit(-18);}

    // Initialize layers
    network[0] = initLayer(network[0], inputSize, inputSize); // Initializes INPUT layer
    network[1] = initLayer(network[1], inputSize, hiddenLayerSize); // Initializes HIDDEN layer
    network[2] = initLayer(network[2], hiddenLayerSize, 1); // Initializes OUTPUT layer

    return network;
}

int existsIn(int number, int array[], int length)
{
    for(int i = 0; i < length; i++)
    {
        if(array[i] == number)
            return 1;
    }    
    return 0;
}

double *feedLayer(Neuron *layer, double *input, int layerSize, int inputSize)
{
    double *result, sum = 0;

    result = (double *)malloc(sizeof(double) * layerSize);
    if(result==NULL){printf("Error while allocating memory for intermediary array");exit(-20);}

    for(int i = 0; i < layerSize; i++)
    {
        for(int j = 0; j < inputSize; j++)
            sum += input[j] * layer[i].weights[j];

        sum += layer[i].bias;        
        result[i] = (double)1.0/(1.0 + pow(M_E, -sum)); // Sigmoid function. M_E is the e constant of math.h
    }
    return result;
}

double feedNetwork(double *imageFeatures, Neuron **network, int inputSize, int hiddenLayerSize)
{
    /* The logic here is that each neuron output of one layer is stored in an intermediary array,
    that becomes the input of each neuron of the subsequent layer */
    
    double *inter12, *inter23, *result, resultValue;

    // Feed layers
    inter12 = feedLayer(network[0], imageFeatures, inputSize, inputSize); // Input layer
    inter23 = feedLayer(network[1], inter12, hiddenLayerSize, inputSize); // Hidden layer
    result = feedLayer(network[2], inter23, 1, hiddenLayerSize); // Output layer

    resultValue = *result;

    // Free resources
    free(inter12);
    free(inter23);
    free(result);

    return resultValue;
}

void freeResources(double **dataset, Neuron **network, int hiddenLayerSize)
{
    // Free dataset
    for(int i = 0; i < DATASET_SIZE; i++)
        free(dataset[i]);
    free(dataset);

    // Free layer 1 weights
   for(int j = 0; j < N_OF_FEATURES; j++)
        free(network[0][j].weights);    

    // Free layer 2 weights
    for(int j = 0; j < hiddenLayerSize; j++)
        free(network[1][j].weights);

    // Free layer 3 weights
    free(network[2][0].weights);

    // Free network layers
    for(int j = 0; j < 3; j++)
        free(network[j]);

    free(network);
}

double getRandomNumber() 
{
    double range = (RANDOM_MAX - RANDOM_MIN); 
    double div = RAND_MAX / range;
    return RANDOM_MIN + (rand() / div);
}

Neuron *initLayer(Neuron *layer, int nOfWeights, int nOfNeurons)
{
    for(int i=0; i < nOfNeurons; i++)
    {
        Neuron neuron;

        // Allocate weights
        neuron.weights = (double *)malloc(sizeof(double) * nOfWeights);
        if(neuron.weights==NULL){printf("Error while allocating memory for neuron weights");exit(-19);}

        for(int j=0; j < nOfWeights; j++)
            neuron.weights[j] = getRandomNumber();

        neuron.bias = getRandomNumber();
        layer[i] = neuron;
    }

    return layer;
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

void shuffle(int *array, int n)
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

void sortIndexes(int *trainingIndexes, int *testingIndexes)
{
    int i = 0, j = 0, n = 0;
    
    while(i < 25) { // Generates 25 grass indexes to use in training set
        int index = rand() % 50;
        if(!existsIn(index, trainingIndexes, 25)) // Check if index is already in the array
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