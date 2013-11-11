#include <stdio.h>
#include <math.h>
#include <convolve.h>
#include <basic_utils.h>

#include "stop.h"


void compute_image_forces (float *image_array, float *stop_array, float *fx, float *fy,  
                           float smoothing, int height, int width)
     
{
  float *temp_array;
  float xm1ym1,xym1,xp1ym1,xm1y,xy,xp1y,xm1yp1,xyp1,xp1yp1;
  float  delta_x,delta_y;
  float Dx, Dy,gradient,speed_value,new_speed_value,image_value;
  int x,y,position,ht,wt;
  FILE *fopen(), *f1;
  delta_x =1.0; delta_y =1.0;
  
  
  CopyIm_FloatToFloat (image_array, stop_array, height, width);  
  
  if (smoothing !=0.0) {
    gauss_conv(stop_array, image_array, 
               smoothing, /* sigma_x */
               smoothing, /* sigma_y */
               2.0, /* range * (kernel size) */
               1.0, /* weight  */
               width,height);
  }
  
  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {

      position = y*width+x;
      
      xm1y = image_array[y*width+(x-1)];
      xp1y = image_array[y*width+(x+1)];
      xym1 = image_array[(y-1)*width+x];
      xyp1 = image_array[(y+1)*width+x];
      
      Dx = (xp1y - xm1y) /2.0;
      Dy = (xyp1 - xym1) /2.0;
      speed_value = sqrt(Dx*Dx+Dy*Dy);
      stop_array[position] = 1.0/(1.0 + speed_value); 
    } 
  } 
  
  adjust_boundaries_float(stop_array, height, width);

  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {
      position = y*width+x;
      
      xm1y = stop_array[y*width+(x-1)];
      xp1y = stop_array[y*width+(x+1)];
      xym1 = stop_array[(y-1)*width+x];
      xyp1 = stop_array[(y+1)*width+x];
      
      Dx = (xp1y - xm1y) /2.0;
      Dy = (xyp1 - xym1) /2.0;
      if (fabs(Dx) > 0.0000001) 
        fx[position] = Dx*pow(fabs(Dx),0.5)/fabs(Dx); 
      else 
        fx[position] = 0.0;
      if (fabs(Dy) > 0.0000001) 
        fy[position] = Dy*pow(fabs(Dy),0.5)/fabs(Dy);
      else
        fy[position] = 0.0;
    } 
  }
    
  adjust_boundaries_float(fx, height, width);
  adjust_boundaries_float(fy, height, width);

#ifdef TEST
  f1 = fopen("results/fy.raw", "w");
  fwrite(fy,sizeof(float),(height*width),f1);
  fclose(f1);
  f1 = fopen("results/fx.raw", "w");
  fwrite(fx,sizeof(float),(height*width),f1);
  fclose(f1);
  
#endif 

}



 
  














