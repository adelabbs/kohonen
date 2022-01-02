#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "kohonen.h"

#define DATA_MAX 200
#define DATA_MIN 0
#define DVP 1
#define DVN 2

int *drawnData = NULL;
int drawnCount = 0;

Data RandomData(size_t n) {
    int *set = (int *)malloc(n * sizeof(int));
    if (set == NULL) {
        perror("Couldn't allocate memory");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        set[i] = (rand() % (DATA_MAX - DATA_MIN)) + DATA_MIN;
    }
    Data data;
    data.set = set;
    data.size = n;
    return data;
}

void DestroyData(Data *data) {
    if (data) {
        int *set = data->set;
        if (set) {
            free(set);
            data->set = NULL;
        }
    }
}

void DestroyDataset(Dataset dataset, size_t datasetSize) {
    if (dataset) {
        for (int i = 0; i < datasetSize; i++) {
            DestroyData(&(dataset[i]));
        }
    }
}

void resetDrawnData(int datasetSize) {
    drawnCount = 0;
    if (drawnData == NULL) {
        drawnData = (int *)malloc(datasetSize * sizeof(int));
        if (drawnData == NULL) {
            perror("Couldn't allocate memory");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < datasetSize; i++) {
        drawnData[i] = 0;
    }
}

void InitialiseSet(Dataset *dataset, int datasetSize, int dataSize) {
    *dataset = (Dataset)malloc(datasetSize * sizeof(Data));
    resetDrawnData(datasetSize);
    for (int i = 0; i < datasetSize; i++) {
        (*dataset)[i] = RandomData(dataSize);
    }
}

/**
 * @brief Draws a random Data from the dataset
 *
 * @param dataset
 * @param datasetSize
 * @return Data
 */
Data SortData(Dataset dataset, size_t datasetSize) {
    if (drawnCount >= datasetSize) { resetDrawnData(datasetSize); }
    int draw = 0;
    int i;
    while (!draw) {
        i = rand() % datasetSize;
        if (drawnData[i] == 0) {
            drawnData[i] = 1;
            draw = 1;
            drawnCount++;
        }
        else {
            draw = 0;
        }
    }
    return dataset[i];
}

/**
 * @brief Prints a Data vector on the console
 *
 * @param data
 */
void PrintData(Data data) {
    int n = data.size;
    for (int i = 0; i < n; i++) {
        printf("%d ", data.set[i]);
    }
    printf("\n");
}

/**
 * @brief Prints the dataset on the console
 *
 * @param dataset
 * @param datasetSize
 */
void PrintDataset(Dataset dataset, size_t datasetSize) {
    for (int i = 0; i < datasetSize; i++) {
        PrintData(dataset[i]);
    }
}

/**
 * @brief Create a Neuron object. The neuron potential and activation values are both set to 0
 *
 * @param i
 * @param j
 * @param x
 * @param y
 * @return Neuron
 */
Neuron CreateNeuron(int i, int j, double *weights) {
    Neuron neuron = {
        .i = i,
        .j = j,
        .weights = weights,
        .pot = 0,
        .act = 0
    };
    return neuron;
}

void PrintNeuronCoordinates(Neuron *neuronSet, size_t nbNeurons, size_t nbWeights) {
    printf("-------------------\n");
    printf("Neuron coordinates:\n");
    printf("-------------------\n");
    for (int k = 0; k < nbNeurons; k++) {
        printf("%d, i = %d, j = %d : [ ", k, neuronSet[k].i, neuronSet[k].j);
        for (int l = 0; l < nbWeights; l++) {
            printf("%f, ", neuronSet[k].weights[l]);
        }
        printf("]\n");
    }
    printf("-------------------\n");
}

/**
 * @brief This implementation of the neuron potential computation uses eucledian distance.
 *
 * @param neuron
 * @param data Must be a 2-D data, as the implementation is computing a eucledian distance
 * @return double
 */
double potential(Neuron neuron, size_t nbWeights, Data data) {
    double s = 0;
    for (int i = 0; i < nbWeights; i++) {
        s += (neuron.weights[i] - data.set[i]) * (neuron.weights[i] - data.set[i]);
    }
    return sqrt(s);
}

/**
 * @brief Computes the neural potential of each neuron in the network for a given data vector
 *
 * @param neuronSet
 * @param nbNeurons
 * @param data
 */
void ComputePotential(Neuron *neuronSet, size_t nbNeurons, size_t nbWeights, Data data) {
    for (int i = 0; i < nbNeurons; i++) {
        neuronSet[i].pot = potential(neuronSet[i], nbWeights, data);
    }
}

/**
 * @brief Computes the activity of each neuron in the network
 *         Call ComputePotential before calling this function, as computing the neuron activity requires neuron potential.
 * @param neuronSet
 * @param nbNeurons
 */
void ComputeActivity(Neuron *neuronSet, size_t nbNeurons) {
    for (int i = 0; i < nbNeurons; i++) {
        neuronSet[i].act = 1 / (1 + neuronSet[i].pot);
    }
}

/**
 * @brief Get the Winning Neuron id
 *
 * @param neuronSet
 * @param nbNeurons
 * @return The id of the winning neuron
 */
int GetWinningNeuron(Neuron *neuronSet, size_t nbNeurons) {
    int winningNeuronId = 0;
    for (int i = 0; i < nbNeurons; i++) {
        if (neuronSet[i].act > neuronSet[winningNeuronId].act) {
            winningNeuronId = i;
        }
    }
    return winningNeuronId;
}

/**
 * @brief Computes the topological distance between a pair of neuron and returns an inhibitory or excitatory coefficient.
 *          This implementation considers i, j components of the neuron to compute their topological distance.
 *
 * @param winner The winning neuron
 * @param neuron
 * @return int returns an inhibitory or excitatory coefficient.
 */
double phi(Neuron winner, Neuron neuron, double alpha, double beta) {
    int wi = winner.i;
    int wj = winner.j;
    int i = neuron.i;
    int j = neuron.j;
    double coef = 0;
    int di = abs(wi - i);
    int dj = abs(wj - j);

    if (wi == i && wj == j) {
        coef = 1;
    }
    else if (di <= DVP && dj <= DVP) {
        coef = alpha;
    }
    else if (di <= DVN && dj <= DVN) {
        coef = -beta;
    }
    return coef;
}

/**
 * @brief Updates all neuron weights for a given iteration. Prerequisite: Determine the winning neuron before calling this function
 *
 * @param neuronSet
 * @param nbNeurons
 * @param data
 * @param winner The winning neuron corresponding to the passde data for the current iteration
 */
void UpdateWeights(Neuron *neuronSet, size_t nbNeurons, size_t nbWeights, Data data, Neuron winner, double epsilon, double alpha, double beta) {
    for (int i = 0; i < nbNeurons; i++) {
        for (int j = 0; j < nbWeights; j++) {
            neuronSet[i].weights[j] = neuronSet[i].weights[j] + epsilon * (data.set[j] - neuronSet[i].weights[j]) * phi(winner, neuronSet[i], alpha, beta);
        }
    }
}