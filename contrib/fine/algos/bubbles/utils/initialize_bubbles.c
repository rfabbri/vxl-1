#include <math.h>
#include <stdio.h>
#include <time.h>

#include <utils.h>
#include <distance_transform.h>
#include <image_processing.h>

#include "initialize_bubbles.h"

Heap heap;
Cedt cedt;

void initialize_bubbles(float *image_array, unsigned char *bubbles_array, int *seeds, 
                        int *intensity_intervals, float stdev, float window_size,
                        int userinit, int height, int width) 
     
{
  int yy,xx,ii;
  
  if (userinit == 0) {
    /* compute the stdev */
    random_bubbles_in_uniform_regions(image_array, bubbles_array,intensity_intervals, 
                                      stdev, window_size, height, width);
  }
  else if (userinit == 2) {
    InitializeImage_UChar(bubbles_array,255,height,width);
    ii=0;
    while (seeds[ii] >=0) {
      yy = seeds[ii];
      xx = seeds[ii+1];
      bubbles_array[yy*width+xx] = 0;
      ii+=2;
    }
  }
  else if (userinit == 3) {
    /* do nothing: assume that bubbles are already initialized in the
      bubbles_array *********/
  }

  /*** initialize heap and cedt */
  

}


void random_bubbles_in_uniform_regions(float *image_array, unsigned char *bubbles_array, 
                                       int *intensity_intervals, float stdev,float window_size,
                                       int height, int width)     
{
  float *stdev_array;
  int im_size,y,x,seed,ii,jj,pos;
  float image_value;
  
  im_size = height*width;
  
  stdev_array = (float *) calloc(im_size,sizeof(float));
  
  compute_stdev_in_a_window (image_array, stdev_array, window_size, 0.0, height, width);
  
  InitializeImage_UChar(bubbles_array,0,height,width);
  
  seed = time (0) - 512413300;
  for(ii=0; ii<im_size/3.0; ii++) {
    y = (int) ((float) height)*rand0(&seed);
    x = (int) ((float) width)*rand0(&seed);
    pos = y*width+x;
    image_value = image_array[pos];
    if (x>1 && x<(width-2) && y> 1 && y<(height-2)) {
      if (stdev_array[pos] <= stdev){
        jj=0;
        while (intensity_intervals[jj] >= 0) {
          if (image_value >= intensity_intervals[jj] && 
              image_value <= intensity_intervals[jj+1])
            bubbles_array[pos] = 1; 
          jj+=2;
        }
      }
    }
  }

  
free(stdev_array);
}


float rand0(int *idum)

{ 
  int k;
  float ans;
  
  *idum ^= MASK;
  k = (*idum)/IQ;
  *idum = IA*(*idum-k*IQ)-IR*k;
  if (*idum < 0) *idum +=IM;
  ans = AM*(*idum);
  *idum ^=MASK;
  return ans;

}










