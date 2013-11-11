#ifdef __cplusplus
extern "C" {
#endif

#ifndef _EM_TBS_
#define _EM_TBS_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define MAX_ITERATIONS 50
#define EPSILON 0.1

typedef struct{
int no_of_classes;
double *mean;
double *sigma;
double *alpha;
}GaussParameter;


double normal_prob(double x, double mu, double sigma);

void gaussMixtureParameterEstimationByEM(double *array, int arr_size,
                     GaussParameter *par);

void freeGaussParameters(GaussParameter *par);

void initializeGaussParameter(GaussParameter *par, int no_of_classes);

#endif

#ifdef __cplusplus
}
#endif

