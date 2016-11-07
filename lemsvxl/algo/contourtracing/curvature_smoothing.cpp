#include <stdio.h>
#include <vcl_cmath.h>

#include "utils.h"

/* #include "curvature_smoothing.h" */
#include "image_processing.h"


void curvature_smoothing(double *image_array, double beta, int steps, int scale, 
                         int height, int width)

{
  double *flux_array;
  int y,x,pos,i;
  FILE *fopen(), *f1;
  double delta_t,delta_x,delta_y,image_value,diffusion_flux_value;
  double max,min,range,max2,min2,range2;
  double flux;
  delta_t = 1.0;

  flux_array = (double *) calloc(height*width,sizeof(double));

  if (scale) {
    max =-99999.0; min = 999999.0; 
    for(y=0; y<height; y++) {  
      for(x=0; x<width; x++) {
        pos = y*width+x;
        image_value = image_array[pos];
        if (max < image_value) 
          max = image_value;
        if (min > image_value) 
          min = image_value;
      }
    }
    range = max-min;
  }

  if (beta != 0.0) {
    for (i=1; i<=steps; i++) {
      compute_diffusion_flux(image_array, flux_array, height, width); 
      
      /* printf("updating.... \n"); */
      for(y=0; y<height; y++) {  
        for(x=0; x<width; x++) {
          pos = y*width+x;
          diffusion_flux_value = flux_array[pos];
#ifdef TEST
          if (fabs(diffusion_flux_value) > 1.0) 
            diffusion_flux_value = diffusion_flux_value/5.0;
#endif 
          flux = delta_t*beta*diffusion_flux_value;
    image_array[pos] += flux;
        } /* for y = 0... */
      }  /* for x = 0 ...*/
    }
  }

  if (scale) {
    max2 =-99999.0; min2 = 99999.0;
    for(y=0; y<height; y++) {  
      for(x=0; x<width; x++) {
        pos =y*width+x;
        image_value = image_array[pos];
        if (image_value > max2) max2 = image_value;
        if (image_value < min2) min2 = image_value;
      }
    }
    
    for(y=0; y<height; y++) {  
      for(x=0; x<width; x++) {
        pos = y*width+x;
        image_value = range*(image_array[pos]-min2)/(max2-min2)+min;
        image_array[pos] = image_value;
      }
    }
  }
  
  free(flux_array);

}

void compute_diffusion_flux(double *surface_array,double *diffusion_array,int height,int width)
    
     
{
  int x,y,position;               /* Loop counters. */
  double xm1ym1, xym1, xp1ym1, xm1y, xy,xp1y,xm1yp1,xyp1,xp1yp1;
  double Dx, Dy, Dxx,Dyy,Dxy;
  double gradient, gradient_squared,curvature_numerator,diffusion_flux_value,curvature;
  double max_curvature_warning, min_meaningful_curvature,min_gradient_warning;
  double surface_value;
  
  max_curvature_warning = 1000;
  min_meaningful_curvature = 0.001;
  min_gradient_warning = 0.001;
  
  for (y=1; y <(height-1); y++) {
    for (x=1; x<(width-1); x++) {

      position = y*width+x;
      surface_value = surface_array[position];
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
  
  adjust_boundaries_double(diffusion_array,height,width);
  
}






