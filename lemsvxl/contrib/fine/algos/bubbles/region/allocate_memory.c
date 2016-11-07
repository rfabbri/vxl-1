#include <math.h>
#include <stdio.h>

#include <distance_transform.h>
#include <bubbles.h>

#include "allocate_memory.h"

void allocate_memory_for_bubbles(Bubble_Deform *BubDeform, int height, int width) 

{
  Bubbles bubbles;
  Region_Statistics *bubst,

  bubbles = BubDeform.bubbles;
  bubst = BubDeform.bubbles.region_statistics;

  bubbles->surface_array = (float *) calloc(height*width,sizeof(float));
  bubbles->level_set_array = (float *) calloc(height*width,sizeof(float));
  bubbles->seeds_array = (unsigned char *) calloc(height*width,sizeof(unsigned char));
  bubbles->region = (int *) calloc(height*width,sizeof(int));
  BubDeform->stop_exp_array = (float *) calloc(height*width,sizeof(float));
  BubDeform->stop_array = (float *) calloc(height*width,sizeof(float));
  BubDeform->image_array = (float *) calloc(height*width,sizeof(float));
  BubDeform->reaction_array = (float *) calloc(height*width,sizeof(float));
  BubDeform->diffusion_array = (float *) calloc(height*width,sizeof(float));
  BubDeform->band_array = (int *) calloc(height*width,sizeof(int));

  bubst->mean = (float *) calloc(size,sizeof(float));
  bubst->stdev = (float *) calloc(size,sizeof(float));
  bubst->sumisq = (float *) calloc(size,sizeof(float));
  bubst->n = (int *) calloc(size,sizeof(int));
   

}

void free_memory_for_bubbles(Bubble_Deform *BubDeform) 

{
  Bubbles bubbles;
  Region_Statistics *bubst,
    
  bubbles = BubDeform.bubbles;
  bubst = BubDeform.bubbles.region_statistics;
  
  free(bubbles->surface_array); 
  free(bubbles->level_set_array);
  free(bubbles->seeds_array); 
  free(bubbles->region);
  free(BubDeform->stop_exp_array):
  free(BubDeform->stop_array);
  free(BubDeform->image_array);
  free(BubDeform->reaction_array);
  free(BubDeform->diffusion_array);
  /* free(BubDeform->band_array); */

  free(bubst->mean); 
  free(bubst->stdev); 
  free(bubst->sumisq); 
  free(bubst->n);
}

void allocate_cedt_data(Cedt *cedt, int height,int width)

{
 cedt->dx = (float *) calloc(height*width,sizeof(float));
 cedt->dy = (float *) calloc(height*width,sizeof(float));
 cedt->dir = (int *)  calloc(height*width,sizeof(int));

}  

void free_cedt_data(Cedt *cedt)
 
{
  free(cedt->dx);
  free(cedt->dy);
  free(cedt->dir);
  
}

k








