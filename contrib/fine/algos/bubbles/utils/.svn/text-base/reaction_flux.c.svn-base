#include <stdio.h>
#include <math.h>

#include <utils.h>

#include "reaction_flux.h"


/*                 The Hamilton-Jacobi Flux in 2D
 */

float hamilton_jacobi_flux (float u_l, float u_r, float u_u, float u_d)
   
{
  float
    min_ul_0,
    max_ur_0,
    min_uu_0,
    max_ud_0,flux_1,flux_2;

  if (u_l < 0)  min_ul_0 = u_l;
  else          min_ul_0 = 0.0;
  
  if (u_r > 0)  max_ur_0 = u_r;
  else          max_ur_0 = 0.0;
  
  if (u_u < 0)  min_uu_0 = u_u;
  else          min_uu_0 = 0.0;
  
  if (u_d > 0)  max_ud_0 = u_d;
  else          max_ud_0 = 0.0;
  flux_1 = sqrt (pow (min_ul_0, 2.0) + pow (max_ur_0, 2.0));
  flux_2 = sqrt (pow (min_uu_0, 2.0) + pow (max_ud_0, 2.0));

  return(0.0 - sqrt (flux_1*flux_1 + flux_2*flux_2)); 
}

    
void reaction_flux_values (float *surface_array, float *reaction_array, int band_size, 
                           float flow_direction, int height, int width)
     
     
{
  int x,y,position; 
  FILE *fopen(), *f1;
  float xm1ym1, xym1, xp1ym1, xm1y, xy, xp1y, xm1yp1,xyp1, xp1yp1;
  float Dxm, Dxp, Dym, Dyp, Dx, Dy, delta_x, delta_y;
  float reaction_flux_value,surface_value;
  
  for (y=1; y<(height-1); y++) {
    for (x=1; x<(width-1); x++) {
      
      position = y*width+x;
      surface_value = surface_array[position];
      
      if (fabs(surface_value) <= band_size) {
        
        xm1ym1 = surface_array[(y-1)*width+(x-1)];
        xym1   = surface_array[(y-1)*width+x];
        xp1ym1 = surface_array[(y-1)*width+(x+1)];
        xm1y = surface_array[y*width+(x-1)];
        xy = surface_array[y*width+x];
        xp1y = surface_array[y*width+(x+1)];
        xm1yp1 = surface_array[(y+1)*width+(x-1)];
        xyp1 = surface_array[(y+1)*width+x];
        xp1yp1 = surface_array[(y+1)*width+(x+1)]; 
        
        Dxm = (xy - xm1y);
        Dxp = (xp1y - xy);
        Dym = (xy - xym1);
        Dyp = (xyp1 - xy);
        
        if (flow_direction < 0) 
          reaction_flux_value = hamilton_jacobi_flux (Dxm, Dxp, Dym, Dyp);
        else if (flow_direction > 0) 
          reaction_flux_value = hamilton_jacobi_flux (-Dxm, -Dxp, -Dym, -Dyp);
        
        reaction_array[position] = reaction_flux_value;
      }
    } /* For (x=0... */
  } /* For (y=0... */
  
  adjust_boundaries_float(reaction_array,height,width);
}
  












