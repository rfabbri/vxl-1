#include <stdio.h>
#include <math.h>
#include <utils.h>
#include <bubbles.h>

#include "image_statistics.h"



void bubbles_mean_and_stdev (Region_Statistics *bubst, int *region, float *image, 
                             int height, int width)


{
  int y,x,pos,label,ii;
  float temp,imagevalue;

  for(ii=0; ii<bubst->number_of_region; ii++) {
    bubst->mean[ii] = 0.0;
    bubst->stdev[ii] = 0.0;
    bubst->n[ii] = 0;
    bubst->sumisq[ii] = 0;
  }
  
  pos=0; 
  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {
      pos = y*width+x;
      label = region[pos];
      if (label != BACKGROUND) {
        bubst->n[label]++;
        bubst->sumisq[label] += image[pos];
      }
    }
  }
  
  for(ii=0; ii<bubst->number_of_region; ii++) {
    if (bubst->n[ii] != 0.0) 
      bubst->mean[ii] = (float) bubst->sumisq[ii]/(float)bubst->n[ii];
    bubst->sumisq[ii] = 0.0;
  }
      
  pos=0; 
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      label = region[pos];
      imagevalue = image[pos];
      if (label != BACKGROUND) {
        temp = (imagevalue-bubst->mean[label])*(imagevalue-bubst->mean[label]);
        bubst->stdev[label] += temp;
        bubst->sumisq[label] += (imagevalue*imagevalue);
      }
    }
  }

#ifdef TEST
  for(ii=0; ii<bubst->region_number; ii++) {
    if (bubst->n[ii] != 0.0)
      bubst->stdev[ii] = sqrt(bubst->stdev[ii]/(float)bubst->n[ii]);
    printf("label =%i, mean =%f and stdev =%f\n", ii,bubst->mean[ii],bubst->stdev[ii]);
  }
#endif TEST  
  
}










