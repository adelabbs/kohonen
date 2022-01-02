#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include <string.h>
#include "base_opengl.h"
#include "ppm.h"
#include "kohonen.h"

#define NB_COLORS 256
#define NB_NEURON NB_COLORS

#define DATA_SIZE 3
#define NB_WEIGHTS DATA_SIZE

#define INPUT_PATH "perroquet.ppm"
#define OUPUT_PATH "compressed256.ppm"

#define NB_NEURON_Y 1 
#define NB_NEURON_X NB_NEURON

#define EPSILON 0.01
#define ALPHA 0.9
#define BETA 0.1

/**
 * @brief Constructs the dataset based on the provided Image
 *
 * @param image
 * @return Dataset
 */
Dataset loadDataset(Image *image) {
    int width = image->x;
    int height = image->y;
    int nbPixels = width * height;
    Dataset dataset = (Data *)malloc(nbPixels * sizeof(Data));
    if (dataset == NULL) EXIT_ON_ERROR("Failed to allocate memory");

    for (int i = 0; i < nbPixels; i++) {
        int *set = (int *)malloc(DATA_SIZE * sizeof(int));
        set[0] = image->data[i].r;
        set[1] = image->data[i].g;
        set[2] = image->data[i].b;

        Data data = {
            .set = set,
            .size = DATA_SIZE
        };

        dataset[i] = data;
    }
    return dataset;
}

/**
 * @brief Fills the neuronset, and arranges neuron topology
 *
 */
void arrangeNeurons(Neuron *neuronset, size_t nbNeurons, size_t nbNeuronsX) {
    int i, j;
    double r, g, b;
    for (int k = 0; k < nbNeurons; k++) {
        i = (k % nbNeuronsX);
        j = (k / nbNeuronsX);
        //Random initial weights:
        r = (double)(rand() % 255);
        g = (double)(rand() % 255);
        b = (double)(rand() % 255);
        double *weights = (double *)malloc(NB_WEIGHTS * sizeof(double));
        if (weights == NULL) EXIT_ON_ERROR("Failed to allocate memory");
        weights[0] = r;
        weights[1] = g;
        weights[2] = b;
        neuronset[k] = CreateNeuron(i, j, weights);
    }
}

void destroyNeurons(Neuron *neuronset, size_t nbNeurons) {
    for (int i = 0; i < nbNeurons; i++) {
        free(neuronset[i].weights);
    }
}

/**
 * @brief Computes and stores all the links between neurons of the network. This implementation creates links in a grid pattern.
 * Here all the neurons are on the same line (i.e. 1D topology, j=0), but this implementation allows the creation of 2D topology.
 *
 */
void createNeuronLinks(Neuron *neuronset, size_t nbNeurons, size_t nbNeuronsX, size_t nbLinks, int **links) {
    int l = 0;
    int neuron = 0;
    while (l < nbLinks) {
        //For each neuron add a link with the neuron on the right and the neuron below if they exist
        //Link with the neuron on the right
        if (((neuron + 1) % nbNeuronsX != 0) && (neuron + 1) < nbNeurons) {
            links[l][0] = neuron;
            links[l][1] = neuron + 1;
            l++;
        }
        //Link with the neuron  below
        if (neuron + nbNeuronsX < nbNeurons) {
            links[l][0] = neuron;
            links[l][1] = neuron + nbNeuronsX;
            l++;
        }
        neuron++;
    }
}


int **malloc_matrix(size_t nrows, size_t ncols) {
    int **matrix = (int **)malloc(nrows * sizeof(int *));
    if (matrix == NULL) EXIT_ON_ERROR("Failed to allocate memory");
    for (int i = 0; i < nrows; i++) {
        matrix[i] = malloc(ncols * sizeof(int));
        if (matrix[i] == NULL) EXIT_ON_ERROR("Failed to allocate memory");
    }
    return matrix;
}


int main(int argc, char const *argv[])
{
    Image *image = NULL;
    Image output;
    int nbPixels;

    Dataset dataset;
    Data currentData;

    Neuron neuronset[NB_NEURON];
    int nbLinks = (NB_NEURON_X - 1) * NB_NEURON_Y + (NB_NEURON_Y - 1) * NB_NEURON_X;
    int **links = malloc_matrix(nbLinks, 2);

    int i;

    image = readPPM(INPUT_PATH);
    dataset = loadDataset(image);
    nbPixels = image->x * image->y;
    resetDrawnData(nbPixels);

    arrangeNeurons(neuronset, NB_NEURON, NB_NEURON_X);
    createNeuronLinks(neuronset, NB_NEURON, NB_NEURON_X, nbLinks, links);

    int winnerId;
    Neuron winner;
    //We train until every pixel of the image has been presented (at random) to the network
    for (i = 0; i < nbPixels; i++) {
        currentData = SortData(dataset, nbPixels);
        ComputePotential(neuronset, NB_NEURON, NB_WEIGHTS, currentData);
        ComputeActivity(neuronset, NB_NEURON);
        winnerId = GetWinningNeuron(neuronset, NB_NEURON);
        winner = neuronset[winnerId];
        UpdateWeights(neuronset, NB_NEURON, NB_WEIGHTS, currentData, winner, EPSILON, ALPHA, BETA);
    }

    output.x = image->x;
    output.y = image->y;
    output.data = (Pixel *)malloc(nbPixels * sizeof(Pixel));
    if (output.data == NULL) EXIT_ON_ERROR("Failed to allocate memory");

    //Once the network has been trained, we determine the compression color of each pixel
    for (i = 0; i < nbPixels; i++) {
        ComputePotential(neuronset, NB_NEURON, NB_WEIGHTS, dataset[i]);
        ComputeActivity(neuronset, NB_NEURON);
        winnerId = GetWinningNeuron(neuronset, NB_NEURON);
        winner = neuronset[winnerId];
        output.data[i].r = (unsigned char) winner.weights[0];
        output.data[i].g = (unsigned char) winner.weights[1];
        output.data[i].b = (unsigned char) winner.weights[2];
    }

    writePPM(OUPUT_PATH, &output);

    DestroyDataset(dataset, nbPixels);
    free(dataset);
    free(links);
    destroyNeurons(neuronset, NB_NEURON);

    return 0;
}