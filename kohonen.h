#ifndef __KOHONEN_H__
#define __KOHONEN_H__
#include <stdlib.h>

typedef struct {
    int *set;
    size_t size; /* Size of the set */
}Data;

typedef Data *Dataset; /* Array of Data vectors */

typedef struct {
    int i;
    int j;
    double *weights; /* Neuron weights */
    double pot; /* Neuron potential*/
    double act; /* Neuron activation value */
}Neuron;

void InitialiseSet(Dataset *dataset, int datasetSize, int dataSize);
Data SortData(Dataset dataset, size_t datasetSize);
void PrintData(Data data);
void DestroyData(Data *data);
void DestroyDataset(Dataset dataset, size_t datasetSize);
void PrintDataset(Dataset dataset, size_t datasetSize);
void resetDrawnData(int datasetSize);

Neuron CreateNeuron(int i, int j, double *weights);
void ComputePotential(Neuron *neuronSet, size_t nbNeurons, size_t nbWeights, Data data);
void ComputeActivity(Neuron *neuronSet, size_t nbNeurons);
int GetWinningNeuron(Neuron *neuronSet, size_t nbNeurons);
void UpdateWeights(Neuron *neuronSet, size_t nbNeurons, size_t nbWeights, Data data, Neuron winner,
    double epsilon, double alpha, double beta);
void PrintNeuronCoordinates(Neuron *neuronSet, size_t nbNeurons, size_t nbWeights);
#endif /* __KOHONEN_H__*/