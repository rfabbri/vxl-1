#ifndef _UTILS_H_
#define _UTILS_H_

#include "dbseg_snakes_snakes.h"

#define SMALL_W1 0.001

void find_matrix_coeffiecents(double **A, float w1, float w2, int size, int type);
void allocate_memory_snakes(Snake *Sn);
void free_memory_snakes(Snake *Sn);
void copy_matrix_to_array(float **matrix, float *array,int size);
void usage_snakes();
void print_snakes_input_values(Snake *sn, InitPars *pars);

#endif /* SNAKES_UTILS */


