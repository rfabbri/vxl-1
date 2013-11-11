#include <stdio.h>
#include <math.h>

#include <utils.h>

#include "diffusion_flux.h"


void diffusion_flux_values (float *surface_array, float *diffusion_array,
                            int band_size, int height, int width)    
     
{
  int x,y,position;               /* Loop counters. */
  float xm1ym1, xym1, xp1ym1, xm1y, xy,xp1y,xm1yp1,xyp1,xp1yp1;
  float Dx, Dy, Dxx,Dyy,Dxy;
  float gradient, gradient_squared,curvature_numerator,diffusion_flux_value,curvature;
  float max_curvature_warning, min_meaningful_curvature,min_gradient_warning;
  float surface_value;
  
  max_curvature_warning = 1000;
  min_meaningful_curvature = 0.001;
  min_gradient_warning = 0.001;
  
  for (y=1; y <(height-1); y++) {
    for (x=1; x<(width-1); x++) {

      position = y*width+x;
      surface_value = surface_array[position];

      if (fabs(surface_value) < band_size) { 
        xm1ym1 = surface_array[(y-1)*width+(x-1)];
        xym1   = surface_array[(y-1)*width+x];
        xp1ym1 = surface_array[(y-1)*width+(x+1)];
        xm1y   = surface_array[y*width+(x-1)];
        xy     = surface_array[y*width+x];
        xp1y   = surface_array[y*width+(x+1)];
        xm1yp1 = surface_array[(y+1)*width+(x-1)];
        xyp1   = surface_array[(y+1)*width+x];
        xp1yp1 = surface_array[(y+1)*width+(x+1)]; 
        
        Dx = (xp1y - xm1y) /2.0;
        Dy = (xyp1 - xym1) /2.0;
        Dxx  = (xp1y + ((-2.0)*xy) + xm1y);
        Dyy  = (xyp1 + ((-2.0)*xy) + xym1);
        Dxy = ((xp1yp1 + xm1ym1) - (xp1ym1 + xm1yp1)) / 4.0;
        
        curvature_numerator = Dyy*Dx*Dx + (-2.0)*Dx*Dy*Dxy + Dxx*Dy*Dy;
        
        gradient_squared = Dx*Dx+Dy*Dy;
        gradient = sqrt(Dx*Dx+Dy*Dy);
        curvature = curvature_numerator/pow (gradient_squared,1.5);
        
        if ((gradient > min_gradient_warning) &&
            (curvature_numerator < max_curvature_warning)) {
          diffusion_flux_value = curvature_numerator / gradient_squared;
        }
        /* Just a warning to say curvature numerator is high. */
        
        else if ((gradient > min_gradient_warning) &&
                 (curvature_numerator >= max_curvature_warning)) {
          diffusion_flux_value = curvature_numerator / gradient_squared;
        } /* Else if */
      
        /* When both gradient and curvature numertaor are small! Do not move! */
        
        else if ((gradient < min_gradient_warning) &&
                 (curvature_numerator < min_meaningful_curvature)) {
    
          diffusion_flux_value = 0.0;
        } /* Else if (gradient < ..... */
        
        
        /* Small gradient, but curvature numerator o.k. The, move by something
         * like mean curvature consider a spherical limit.
         */
        else {
          
          diffusion_flux_value = (Dxx+Dyy)/ 2.0;
        } /* Else */
        
        diffusion_array[position]= diffusion_flux_value;
        
      } /* For (x=0... */
    } /* For (y=0... */
  }

  adjust_boundaries_float(diffusion_array,height,width);

}















