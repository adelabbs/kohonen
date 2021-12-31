#ifndef __KOHONEN_H__
#define __KOHONEN_H__
#include <stdlib.h>

typedef struct {
    int *set;
    size_t size;
}Data;

typedef Data *Dataset;

typedef struct {
    int i;
    int j;
    double x;
    double y;
    double pot;
    double act;
}Neuron;

void InitialiseSet(Dataset *dataset, int datasetSize, int dataSize);
Data SortData(Dataset dataset, size_t datasetSize);
void PrintData(Data data);
void DestroyData(Data *data);
void DestroyDataset(Dataset dataset, size_t datasetSize);
void PrintDataset(Dataset dataset, size_t datasetSize);


Neuron CreateNeuron(int i, int j, double x, double y);
void ComputePotential(Neuron *neuronSet, size_t nbNeurons, Data data);
void ComputeActivity(Neuron *neuronSet, size_t nbNeurons);
int GetWinningNeuron(Neuron *neuronSet, size_t nbNeurons);
void UpdateWeights(Neuron *neuronSet, size_t nbNeurons, Data data, Neuron winner);
void PrintNeuronCoordinates(Neuron *neuronSet, size_t nbNeurons);
#endif /* __KOHONEN_H__*/