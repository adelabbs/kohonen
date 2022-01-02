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

#define NB_COLORS 16
#define NB_NEURONS NB_COLORS

void train(int nb_iterations, Neuron *neuronset, size_t nb_neurons, int alpha, int beta, int epsilon, Dataset dataset, size_t datasetSize);

int main(int argc, char const *argv[])
{
    Image *image = NULL;
    image = readPPM("perroquet.ppm");
    return 0;
}
