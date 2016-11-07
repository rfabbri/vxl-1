#include <stdio.h>
#include <math.h>

#include <utils.h>
#include <distance_transform.h>
#include <bubbles.h>

#include "deform_RD_bubbles.h"
#include "allocate_memory.h"


void deform_RD_bubbles(Bubbles *bubbles, float alpha, float beta, 
                    int iteration, float band_size, int height, float width) 

{
  int y,x,position;
  float reaction_flux_value, diffusion_flux_value, total_flux_value;
  float speed_value,delta_x,delta_y,delta_t,g_smoothing;
  int reinitialize;
  int diffusion_on;
  extern Heap heap;
  extern Cedt cedt;
  FILE *fopen(), *f1;
  
  /* initialization ********/
  if (iteration == 1) {
    get_level_set (bubbles->surface_array, bubbles->reaction_array, -band_size,band_size,
                   0,(band_size+3.0), height, width);
    narrow_band_boundaries(bubbles->surface_array,bubbles->reaction_array,
                           bubbles->band_boundary,band_size, height, width);
    cedt.surface = bubbles->level_set_array;
    g_smoothing = 0.0;
    delta_t = 1.0; delta_x = delta_y = 1.0;
  }
  

  diffusion_on = (int) fabs((1.0/alpha)); 
  if (iteration <= diffusion_on)
    beta = beta*pow((float) ((float) iteration /(float) diffusion_on), 3.0);
  else beta =  beta;
 


  /* expand the stop function by using CEDT to all level sets in narrow band */
  speed_expansion_by_cedt(&heap, &cedt, bubbles->level_set_array, bubbles->stop_array, 
                          bubbles->stop_exp_array, band_size, height, width);
   
  /* see if the bubbles are close to narrow band boundaries. If they are then use 
    the fast marching algorithm to re-initialize the surface */
  get_level_set (bubbles->surface_array, bubbles->level_set_array, 0.0, 0.0,0,
                 band_size, height, width);
  reinitialize = zero_level_in_narrow_band(bubbles->level_set_array,bubbles->band_boundary,0.0,
                                           height,width);
  if (reinitialize) { 
    reinitialize_surface_by_fm(bubbles->surface_array,bubbles->level_set_array,&heap,
                               band_size, height,width);
    get_level_set (bubbles->surface_array, bubbles->level_set_array, -band_size,
                   band_size, 0,(band_size+3.0), height, width);
    
    narrow_band_boundaries(bubbles->surface_array,bubbles->level_set_array,
                           bubbles->band_boundary,band_size, height, width);
  } 
 

  if (alpha != 0.0) {
    reaction_flux_values(bubbles->surface_array,bubbles->reaction_array,band_size,alpha,
                         height,width);
    /* printf("getting reaction\n"); */
  }
  
  
  /* smooth the shape by gaussian */
  if (g_smoothing != 0.0 && beta != 0.0) {
    gauss_conv(bubbles->surface_array, bubbles->level_set_array, g_smoothing, g_smoothing,
               2.0, 1.0, height, width);
    diffusion_flux_values(bubbles->level_set_array,bubbles->diffusion_array,band_size,
                          height,width);
  }
  
  if (beta != 0.0) { 
    diffusion_flux_values(bubbles->surface_array,bubbles->diffusion_array,band_size,
                          height,width);
    /*    printf("getting diffusion\n");  */
    }
  
  /* printf("updating.... \n"); */
  for(y=1; y<(height-1); y++) {  
    for(x=1; x<(width-1); x++) {
      position = y*width+x;
      if (fabs(bubbles->surface_array[position]) <band_size) {
        reaction_flux_value = bubbles->reaction_array[position];
        if (beta != 0.0) 
          diffusion_flux_value = bubbles->diffusion_array[position];
        else diffusion_flux_value = 0.0;
        total_flux_value = alpha*reaction_flux_value+beta*diffusion_flux_value;
        speed_value = bubbles->stop_exp_array[position]; 
        bubbles->surface_array[position] += delta_t*speed_value*total_flux_value;
      } /* for y = 0... */
    }  /* for x = 0 ...*/
  }
  
  adjust_boundaries_float(bubbles->surface_array, height, width); 
  

}













