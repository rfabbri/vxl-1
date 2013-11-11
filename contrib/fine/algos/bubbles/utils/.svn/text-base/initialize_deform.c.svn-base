#include <math.h>
#include <stdio.h>

#include <utils.h>
#include <distance_transform.h>
#include <bubbles.h>

#include "initialize_deform.h"

void initialize_deform(Bubble_Deform *BubDeform, float *image_array, float *stop_array, 
                       float *surface_array, int edge_driven, float alpha, float beta,
               float gamma, float delta_y,float delta_x,float band_size,
                       int height, int width)    
{       
  int size;
  Bubbles bubbles;
  Region_Statistics bubst;
  
  BubDeform->alpha = alpha;
  BubDeform->beta = beta;
  BubDeform->gamma = gamma;
  BubDeform->delta_x = delta_x;
  BubDeform->delta_y = delta_y;
  BubDeform->band_size = band_size;
  BubDeform->height = height;
  BubDeform->width = width;
  

  BubDeform->bubbles.surface_array = (float *) calloc(height*width,sizeof(float));
  CopyIm_FloatToFloat(surface_array,BubDeform->bubbles.surface_array,height,width);
  BubDeform->image_array = (float *) calloc(height*width,sizeof(float));
  CopyIm_FloatToFloat(image_array,BubDeform->image_array,height,width);
  BubDeform->stop_array = (float *) calloc(height*width,sizeof(float));
  CopyIm_FloatToFloat(stop_array,BubDeform->stop_array,height,width);
 
  /* now allocate space for the neccesarry arrays */ 
  allocate_heap_data(&BubDeform->heap,height,width);

  BubDeform->cedt.dx = (float *) calloc(height*width,sizeof(float));
  BubDeform->cedt.dy = (float *) calloc(height*width,sizeof(float));
  BubDeform->cedt.dir = (int *)  calloc(height*width,sizeof(int));
  
  BubDeform->reaction_array = (float *) calloc(height*width,sizeof(float));
  BubDeform->diffusion_array = (float *) calloc(height*width,sizeof(float));
  BubDeform->stop_exp_array = (float *) calloc(height*width,sizeof(float));
  BubDeform->bubbles.level_set_array = (float *) calloc(height*width,sizeof(float));
  BubDeform->band_array = (int *) calloc(height*width,sizeof(int));

  bubbles = BubDeform->bubbles;
  bubst = BubDeform->bubbles.region_statistics;
  
  BubDeform->bubbles.height = height;
  BubDeform->bubbles.width = width;


  if (edge_driven == 0) {
    BubDeform->bubbles.region_array = (int *) calloc(height*width,sizeof(int)); 

    BubDeform->bubbles.region_statistics.number_of_region =
      fill_bubbles(surface_array, BubDeform->bubbles.region_array,
                   0,height,width);
    size = (BubDeform->bubbles.region_statistics.number_of_region+2);
    /* first allocate memory for statistical arrays */
    BubDeform->bubbles.region_statistics.mean = (float *) calloc(size,sizeof(float));
    BubDeform->bubbles.region_statistics.stdev = (float *) calloc(size,sizeof(float));
    BubDeform->bubbles.region_statistics.sumisq = (float *) calloc(size,sizeof(float));
    BubDeform->bubbles.region_statistics.n = (int *) calloc(size,sizeof(int));
    /* find the mean and stdev of each bubbles */
    bubbles_mean_and_stdev (&BubDeform->bubbles.region_statistics, 
                            BubDeform->bubbles.region_array,image_array,height,width);
  }                                  

/*   fprintf(stderr,"Stdev in Tek's code = %f %f %f %f\n",BubDeform->bubbles.region_statistics.stdev[0],BubDeform->bubbles.region_statistics.stdev[1],BubDeform->bubbles.region_statistics.stdev[2],BubDeform->bubbles.region_statistics.stdev[3]); */

  get_level_set (bubbles.surface_array, bubbles.level_set_array, -band_size,band_size,
                 0,(band_size+3.0), height, width);
  narrow_band_boundaries(bubbles.surface_array,bubbles.level_set_array,
                         BubDeform->band_array,band_size, height, width);
  BubDeform->cedt.surface = bubbles.level_set_array;

/*   fprintf(stderr,"cedt.surface and level_set_array in Teks's code =\n %f %f %f %f\n",bubbles.level_set_array[0],bubbles.level_set_array[1],BubDeform->cedt.surface[2],BubDeform->cedt.surface[3]); */
  
}








