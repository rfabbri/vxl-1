#include <stdio.h>
#include <math.h>

#include <utils.h>
#include <distance_transform.h>

#include "region.h" 
#include "allocate_memory.h"


void init_bubbles_mean_and_stdev (BubStatistics *bubst, int *region, float *image, 
                                  int height, int width)


{
  int y,x,pos,label,ii;
  float temp,imagevalue;

  /* first allocate memory for statistical arrays */
  allocate_memory_statistics(bubst,bubst->region_number);
 

  for(ii=0; ii<bubst->region_number; ii++) {
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
  
  for(ii=0; ii<bubst->region_number; ii++) {
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



 
void find_new_mean_and_stdev(BubStatistics *bubst, int label, float imagevalue)
 
{
 
  float NN,temp_value;
 
  NN = (float) bubst->n[label];
  
  /*find new mean */
  bubst->mean[label] = (NN*bubst->mean[label]+imagevalue)/(NN+1);
  bubst->n[label]++;
  NN = (float) bubst->n[label];
 
  /* find new stdev */
  temp_value = (bubst->sumisq[label]/NN) - (bubst->mean[label]*bubst->mean[label]) + 
    (imagevalue*imagevalue)/NN;
  if (temp_value <=  1E-5)
    temp_value = 0.1;
 
  bubst->stdev[label] = sqrt(temp_value);
  bubst->sumisq[label] += (imagevalue*imagevalue);
 
}

void update_statistics_from_level_set (float *surface_array,int *region, float *image, 
                                       BubStatistics *bubst, float band_size,
                                       int height, int width) 

{ 
  int x_pos[4] = {1,1,0,-1};
  int y_pos[4] = {0,1,1,1};
  int ii,z,y,x,new_position,pos,pos2;
  float interpolation,point_value,neighbors_value,level_set_value,zero_crossing;
  int position,zz,yy,xx,direction;
  float boundary_intensity,non_boundary_intensity;  
  float imagevalue;


  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {
      pos = y*width+x;
      if (surface_array[pos] >=0.0 && region[pos] == BACKGROUND) {
        for(yy=y-1; yy<(y+2); yy+=2) {
          for(xx=x-1; xx<(x+2); xx+=2) {
            pos2 =yy*width+xx;
            if (surface_array[pos2] >=0.0 && region[pos2] != BACKGROUND) {
              boundary_intensity = region[pos2];
              imagevalue = image[pos];
              find_new_mean_and_stdev(bubst,boundary_intensity,imagevalue); 
              region[pos] = boundary_intensity;
              goto out;
            }
          }
        }
      out:
      }
    }
  }
 adjust_boundaries_int(region, height, width); 

}
                                               








