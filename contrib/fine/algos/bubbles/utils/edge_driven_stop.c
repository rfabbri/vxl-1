#include <stdio.h>
#include <math.h>
#include <utils.h>

#include "edge_driven_stop.h"


/* comput the stop function (1/(1+|grad(I)|^k)  */

void edge_driven_stop_values(float *image_array,float *stop_array,float k,int height,int width)
     
{
  float xm1ym1,xym1,xp1ym1,xm1y,xy,xp1y,xm1yp1,xyp1,xp1yp1;
  float  delta_x,delta_y;
  float Dx, Dy,gradient,speed_value,new_speed_value,image_value;
  int x,y,position;
  FILE *fopen(), *f1;
  delta_x =1.0; delta_y =1.0;
  
  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {
      position = y*width+x;
      
      xm1y = image_array[y*width+(x-1)]/delta_x;
      xp1y = image_array[y*width+(x+1)]/delta_x;
      xym1 = image_array[(y-1)*width+x]/delta_y;
      xyp1 = image_array[(y+1)*width+x]/delta_y;
      
      Dx = (xp1y - xm1y) /2.0;
      Dy = (xyp1 - xym1) /2.0;
      
      gradient = sqrt(Dx*Dx +Dy*Dy);
      image_value = image_array[y*width+x];
      /*  image_gr_array[position] = gradient; */
      speed_value = 1.0/(1.0 + pow(fabs(gradient),k));
      new_speed_value =  1.0/(1.0 + exp((image_value -100.0)/4.0));
      stop_array[position] = speed_value;
    } 
  } 

  adjust_boundaries_float(stop_array, height, width);

}



 
  














