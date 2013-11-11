#include <math.h>
#include <stdio.h>

#include <utils.h>


int fill_bubbles(float *surface, int *region, int boundary, int height, int width)

{
  int *mask; 
  int position, new_position;
  int y,x,z,pos,seeds_pos,ii,label;
  float point_value,neighbors_value,zero_crossing,interpolation;
  FILE *fopen(), *f1;
  struct window {
    int x0, y0;
    int x1, y1;
  }my_window;  

  float small_number = 0.0;

  int x_pos[4] = {1,1,0,-1};
  int y_pos[4] = {0,1,1,1};

  mask = (int *) calloc(height*width,sizeof(int));
  
  my_window.x0 = 0;
  my_window.y0 = 0;
  my_window.x1 = width-1;
  my_window.y1 = height-1;
      
  
  for (y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      region[pos] = -1.0;
      if (surface[pos] >= 0.0) 
        mask[pos] = 0.0;
      else 
        mask[pos] = 255.0;
      if (boundary)       
        if ((y == 1 || x == 1 || y == (height-2) || x == (width-2)) && (surface[pos] < 0.0)) 
          region[pos] = 1;
    }
  }
  
  label =2;
  for (y=2; y<(height-2); y++) {
    for(x=2; x<(width-2); x++) {
      pos = y*width+x;
      if ((region[pos] == -1.0) && (surface[pos] > 0.0)) {
        /*  printf("x = %i and y =%i and label =%i\n", x,y,label);  */
        fill(x, y, width, my_window, region, mask, label);
        label++;
      }
    }
  }
  
  /* adjust the filled regions around zero level set */
  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {
      position = y*width+x;
      point_value =  surface[position]+small_number;
      for(ii=0; ii<4; ii++) {
        new_position = (y+y_pos[ii])*width +(x+x_pos[ii]);
        neighbors_value = surface[new_position]+small_number;
        zero_crossing = point_value*neighbors_value;
        
        if (zero_crossing <= 0.0) {
          interpolation = fabs(point_value) - fabs(neighbors_value);
          
          if (interpolation <= 0.0) {
            if (point_value < 0.0) {
              region[position] = region[new_position];
            }
          }
          else if (interpolation > 0.0) {
            if (neighbors_value < 0.0) {
              region[new_position] = region[position];
              }
          }
        }
      }
    }
  }
  
  adjust_boundaries_int(region, height, width); 
  
  free(mask);
  
  return(label);
}








