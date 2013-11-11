#include <math.h>
#include <stdio.h>

#include "utils.h"


void find_matrix_coeffiecents(double **A, float w1, float w2, int size, int type)

{
  int i,j,pos;

  for(i=2; i<size-2; i++) {
    for(j=0; j<size; j++) {
      A[i][j] = 0;
      A[i][i-2] = w2;
      A[i][i-1] = -w1-4.0*w2;
      A[i][i] = 2.0*w1+6.0*w2;
      A[i][i+1] = -w1-4.0*w2;
      A[i][i+2] = w2;
    }
  }

  /* i =0 row */
  A[0][0] = 2.0*w1+6.0*w2;
  A[0][1] =  -w1-4.0*w2;
  A[0][2] = w2;
  for(j=3; j<size; j++) 
    A[0][j] = 0.0;

  /* i =1 */
  A[1][0] = -w1-4.0*w2;
  A[1][1] =  2.0*w1+6.0*w2;
  A[1][2] = -w1-4.0*w2;
  A[1][3] = w2;
  for(j=4; j<size; j++) 
    A[1][j] = 0.0;

  /* i =n-2 */
  
  A[size-2][size-4] = w2;
  A[size-2][size-3] =  -w1-4.0*w2; 
  A[size-2][size-2] = 2.0*w1+6.0*w2;
  A[size-2][size-1] = -w1-4.0*w2;
  for(j=0; j<size-4; j++) 
    A[size-2][j] = 0.0;

  
  A[size-1][size-3] = w2;
  A[size-1][size-2] =  -w1-4.0*w2; 
  A[size-1][size-1] = 2.0*w1+6.0*w2;
  for(j=0; j<size-3; j++) 
    A[size-1][j] = 0.0;
  

  if (type == CLOSECONTOUR) {
    A[0][size-2] = w2;
    A[0][size-1] = -w1-4.0*w2;
    A[1][size-1] = w2;

    A[size-2][0] = w2;
    A[size-1][0] = -w1-4.0*w2; 
    A[size-1][1] = w2;
  }

  else if (type == OPENCONTOUR) {
    w1 = SMALL_W1;
    A[0][0] = w1;
    A[0][1] =  -w1;
    A[0][2] = 0.0; 
    
    A[1][0] = -w1-2.0*w2;
    A[1][1] =  2.0*w1+4.0*w2;
    A[1][2] = -w1-2.0*w2;
    A[1][3] = 0.0;
    
    A[size-2][size-4] = 0.0;
    A[size-2][size-3] =  -w1-2.0*w2; 
    A[size-2][size-2] = 2.0*w1+4.0*w2;
    A[size-2][size-1] = -w1-2.0*w2;
    
    A[size-1][size-3] = 0.0;
    A[size-1][size-2] =  -w1;
    A[size-1][size-1] = w1;
  }


}


void copy_matrix_to_array(float **matrix, float *array,int size)

{
  int i,j,pos;
  
  for(i=0; i<size; i++) {
    for(j=0; j<size; j++) {
      pos = i*size+j;
      array[pos] = matrix[i][j];
    }
  }

}
 

void allocate_memory_snakes(Snake *Sn)

{
  int imsize = Sn->h*Sn->w;

  Sn->image = (float *) calloc(imsize,sizeof(float));
  Sn->stop = (float *) calloc(imsize,sizeof(float));
  Sn->fx = (float *) calloc(imsize,sizeof(float));
  Sn->fy = (float *) calloc(imsize,sizeof(float));
  
}

void free_memory_snakes(Snake *Sn)

{
  free(Sn->image);
  free(Sn->stop);
  free(Sn->fx);
  free(Sn->fy);
}

void usage_snakes ()
{
  
  printf("Snakes Usage:
Snakes <inputimage> <snakesimage> <outputimage> 
Optional Inputs:
         -w1 [0.3]
         -w2  [0.3]]
         -smoothing [1.0]
         -delta_t [0.5]
         -steps [1]
         -how_often_save [1]\n");
}

void print_snakes_input_values(Snake *sn, InitPars *pars) 

{

printf("
-inputimagee               %s 
-outputimage               %s
-w1                        %lf 
-w2                        %lf 
-smoothing                 %lf
-gamma                     %f
-steps                     %i 
-start_from                %i
-how_often_save            %i\n",
pars->inimage,
pars->outimage,
sn->w1,
sn->w2,
sn->smoothing,       
sn->dt,       
pars->steps,
pars->how_often_save);

}


















