#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int xx_loc[4] = {1,1,0,-1};
int yy_loc[4] = {0,1,1,1};


void get_level_set_surface(double *surface_array, double *level_set_array, 
               double level, int boundary_value, int interpolate, 
               int band_size, int height, int width)
              
     
{ 
  int z,y,x,pos;
  double surface_value,neighbors_value,level_set_value,zero_crossing;
  int position,zz,yy,xx,direction;
  double boundary_intensity,non_boundary_intensity;  
  boundary_intensity = 0.0;
  non_boundary_intensity = 255.0;
  
  /* initialize  level_set_array  */
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      level_set_array[y*width+x] = non_boundary_intensity;
    }
  }
  
  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {
      pos = y*width+x;
      surface_value = surface_array[pos]-level;
      if (fabs(surface_value) <= band_size) 
        find_zero_crossing(level_set_array, surface_array, surface_value, 
                           level, boundary_value, y, x, interpolate, height, width);
    }
  }

  adjust_boundaries_double(level_set_array, height, width); 
  
}

void find_zero_crossing_double(double *level_set_array, double *surface_array, 
                   double point_value, 
                   double level, int boundary_value, int y, int x, 
                   int interpolate, int height, int width)
     
     
{ 
  int ii,yy,xx;
  int new_position,position;
  double neighbors_value,zero_crossing,interpolation;

  position = y*width+x;

  for(ii=0; ii<4; ii++) {
    yy = y + yy_loc[ii];
    xx = x + xx_loc[ii];
    new_position = yy*width +xx;
    neighbors_value = surface_array[new_position]- level;
    zero_crossing = point_value*neighbors_value;
    interpolation = fabs(point_value) - fabs(neighbors_value);
    if (zero_crossing <= 0.0) {
      if (interpolate)
        linear_interpolation(level_set_array,interpolation,boundary_value,position,
                             new_position); 
      else {
        if (point_value >= 0) {
          level_set_array[position] = boundary_value;
          level_set_array[new_position] = -1.0*boundary_value;
        }
        else {
          level_set_array[position] = -1.0*boundary_value;
          level_set_array[new_position] = boundary_value;
        }
      }
    }
  }
}


void linear_interpolation_double(double *level_set_array, double interpolation, 
                 double boundary_value,
                 int position, int new_position)

{
  if (interpolation < 0.0) 
    level_set_array[position] = boundary_value;
  else if (interpolation > 0.0) 
    level_set_array[new_position] = boundary_value;
  else {
    level_set_array[new_position] = boundary_value;  
    level_set_array[position] = boundary_value;
  }

}









#ifdef __cplusplus
}
#endif





