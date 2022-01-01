#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "kohonen.h"

#define DATA_MAX 200
#define DATA_MIN 0
#define DVP 1
#define DVN 2
#define ALPHA 0.9
#define BETA 0.1
#define EPSILON 0.02

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
    else {
        for (int i = 0; i < datasetSize; i++) {
            drawnData[i] = 0;
        }
    }
}

void InitialiseSet(Dataset *dataset, int datasetSize, int dataSize) {
    *dataset = (Dataset)malloc(datasetSize * sizeof(Data));
    resetDrawnData(datasetSize);
    for (int i = 0; i < datasetSize; i++) {
        (*dataset)[i] = RandomData(dataSize);
    }
}

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
Neuron CreateNeuron(int i, int j, double x, double y) {
    Neuron neuron = {
        .i = i,
        .j = j,
        .x = x,
        .y = y,
        .pot = 0,
        .act = 0
    };
    return neuron;
}

void PrintNeuronCoordinates(Neuron *neuronSet, size_t nbNeurons) {
    printf("-------------------\n");
    printf("Neuron coordinates:\n");
    printf("-------------------\n");
    for (int k = 0; k < nbNeurons; k++) {
        printf("%d, i = %d, j = %d : [%f, %f]\n", k, neuronSet[k].i, neuronSet[k].j, neuronSet[k].x, neuronSet[k].y);
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
double potential(Neuron neuron, Data data) {
    int *set = data.set;
    return sqrt((neuron.x - set[0]) * (neuron.x - set[0]) + (neuron.y - set[1]) * (neuron.y - set[1]));
}

/**
 * @brief Computes the neural potential of each neuron in the network for a given data vector
 *
 * @param neuronSet
 * @param nbNeurons
 * @param data
 */
void ComputePotential(Neuron *neuronSet, size_t nbNeurons, Data data) {
    for (int i = 0; i < nbNeurons; i++) {
        neuronSet[i].pot = potential(neuronSet[i], data);
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
double phi(Neuron winner, Neuron neuron) {
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
        coef = ALPHA;
    }
    else if (di <= DVN && dj <= DVN) {
        coef = -BETA;
    }
    return coef;
}

void UpdateWeights(Neuron *neuronSet, size_t nbNeurons, Data data, Neuron winner) {
    for (int i = 0; i < nbNeurons; i++) {
        neuronSet[i].x = neuronSet[i].x + EPSILON * (data.set[0] - neuronSet[i].x) * phi(winner, neuronSet[i]);
        neuronSet[i].y = neuronSet[i].y + EPSILON * (data.set[1] - neuronSet[i].y) * phi(winner, neuronSet[i]);
    }
}