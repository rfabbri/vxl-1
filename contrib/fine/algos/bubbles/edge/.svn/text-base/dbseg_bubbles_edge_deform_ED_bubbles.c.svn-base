#include <stdio.h>
#include <math.h>

#include <utils.h>
#include <distance_transform.h>
#include <bubbles.h>

#include "deform_ED_bubbles.h"


void deform_ED_bubbles(Bubble_Deform *BubDeform, int iteration) 

{
  Bubbles bubbles;
  int height,width;
  int y,x,position;
  float band_size;
  float reaction_flux_value, diffusion_flux_value, total_flux_value;
  float speed_value,delta_x,delta_y,delta_t,g_smoothing;
  int reinitialize;
  int diffusion_on;
  extern Heap heap;
  extern Cedt cedt;
  FILE *fopen(), *f1;
  
  bubbles = BubDeform->bubbles;
  height = BubDeform->height;
  width = BubDeform->width;
  band_size =  BubDeform->band_size;

  g_smoothing = 0.0;
  delta_t = 1.0; delta_x = delta_y = 1.0;
  
  diffusion_on = (int) fabs((1.0/BubDeform->alpha)); 
  if (iteration <= diffusion_on)
    BubDeform->beta = BubDeform->beta*pow((float) ((float) iteration /(float) diffusion_on), 
                                          3.0);
  else BubDeform->beta =  BubDeform->beta;
  
  /* see if the bubbles are close to narrow band boundaries. If they are then use 
    the fast marching algorithm to re-initialize the surface */
  get_level_set (bubbles.surface_array, bubbles.level_set_array, 0.0, 0.0,0,
                 band_size, height, width);
  reinitialize = zero_level_in_narrow_band(bubbles.level_set_array,BubDeform->band_array,
                                           0.0,height,width);
  if (reinitialize) { 
    reinitialize_surface_by_fm(bubbles.surface_array,bubbles.level_set_array,&BubDeform->heap,
                               band_size, height,width);
    get_level_set (bubbles.surface_array, bubbles.level_set_array, -band_size,
                   band_size, 0,(band_size+3.0), height, width);
    
    narrow_band_boundaries(bubbles.surface_array,bubbles.level_set_array,
                           BubDeform->band_array,band_size, height, width);
  } 
 

  get_level_set (bubbles.surface_array, bubbles.level_set_array, 0.0, 0.0, 1, band_size, 
                 height,  width); 

  /* expand the stop function by using CEDT to all level sets in narrow band */
  speed_expansion_by_cedt(&BubDeform->heap, &BubDeform->cedt, bubbles.level_set_array, 
                          BubDeform->stop_array,BubDeform->stop_exp_array, band_size,
                          height, width);
  
   
  if (BubDeform->alpha != 0.0) {
    reaction_flux_values(bubbles.surface_array,BubDeform->reaction_array,band_size,
                         BubDeform->alpha,height,width);
    /* printf("getting reaction\n"); */
  }
  
  
  /* smooth the shape by gaussian */
  if (g_smoothing != 0.0 && BubDeform->beta != 0.0) {
    gauss_conv(bubbles.surface_array, bubbles.level_set_array, g_smoothing, g_smoothing,
               2.0, 1.0, height, width);
    diffusion_flux_values(bubbles.level_set_array,BubDeform->diffusion_array,band_size,
                          height,width);
  }
  
  if (BubDeform->beta != 0.0) { 
    diffusion_flux_values(bubbles.surface_array,BubDeform->diffusion_array,band_size,
                          height,width);
    /*    printf("getting diffusion\n");  */
    }
  
  /* printf("updating.... \n"); */
  for(y=1; y<(height-1); y++) {  
    for(x=1; x<(width-1); x++) {
      position = y*width+x;
      if (fabs(bubbles.surface_array[position]) <band_size) {
        reaction_flux_value = BubDeform->reaction_array[position];
        if (BubDeform->beta != 0.0) 
          diffusion_flux_value = BubDeform->diffusion_array[position];
        else diffusion_flux_value = 0.0;
        total_flux_value = BubDeform->alpha*reaction_flux_value+
          BubDeform->beta*diffusion_flux_value;
        speed_value = BubDeform->stop_exp_array[position]; 
        bubbles.surface_array[position] += delta_t*speed_value*total_flux_value;
      } /* for y = 0... */
    }  /* for x = 0 ...*/
  }
  
  adjust_boundaries_float(bubbles.surface_array, height, width); 
  

}













