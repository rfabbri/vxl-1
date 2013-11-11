#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdio.h>
#include "dbseg_srgtek_grow.h"
#include "dbseg_srgtek_find_contours.h"
#define SSLEND 9999.0

void grow_regions(int *seed_array, unsigned char *boundary_array,
          int *aux_seed_array, unsigned char *image_array, 
          float *ssl_array, int no_of_seeds, 
          float *mean_averages, int height, int width)

{
  int pos,y,x,seed_label,position;
  float new_total,delta_z;
  FILE *fopen(), *f1;


pos=0;
  while (ssl_array[pos] != SSLEND) {
    y = (int) ssl_array[pos+1];
    x = (int) ssl_array[pos+2];
    seed_label = (int) ssl_array[pos+3];
    position = y*width+x;
    delta_z = ssl_array[pos];

    if ((seed_array[(y-1)*width+x-1]==seed_label)&&
    (seed_array[(y-1)*width+x]==seed_label) && 
        (seed_array[(y-1)*width+x+1]==seed_label)&&
    (seed_array[y*width+x+1]==seed_label) &&
    (seed_array[(y+1)*width+x+1]==seed_label)&&
    (seed_array[(y+1)*width+x]==seed_label) &&
    (seed_array[(y+1)*width+x-1]==seed_label)&&
    (seed_array[y*width+x-1]==seed_label)) {
     seed_array[y*width+x] = seed_label;
     find_new_mean(mean_averages, image_array, seed_label, position); 
     pull_from_ssl(ssl_array);
   }
   else {
     add_to_region(seed_array,boundary_array,mean_averages,image_array,delta_z,
           seed_label,no_of_seeds,y,x,height,width);
     pull_from_ssl(ssl_array); 
     put_neighbors_into_ssl(ssl_array,mean_averages,seed_array,aux_seed_array,image_array,
                seed_label,y,x,height,width); 
    }

  }

}
      
void pull_from_ssl(float *ssl_array)

{
  int pos;

  pos=0;
  while(ssl_array[pos+4] != -1.0) {
    ssl_array[pos] = ssl_array[pos+4];
    pos++;
  }
  ssl_array[pos] = -1.0;
  ssl_array[pos] = 0.0; pos++;
  ssl_array[pos] = 0.0; pos++;
  ssl_array[pos] = 0.0; pos++;
}

void put_neighbors_into_ssl(float *ssl_array, float *mean_averages, int *seed_array, 
                int *aux_seed_array, unsigned char *image_array, 
                int seed_label, 
                int y, int x, int height, int width)
     
{
  int position;

  float right_mean,delta_z;
  right_mean = mean_averages[seed_label*3-3];

  position = (y-1)*width+x-1;
  if ((seed_array[position] == 0) && (aux_seed_array[position] != INSSL)) {
    delta_z = fabs((float)image_array[position]-right_mean);
    put_into_ssl(delta_z,ssl_array,(y-1),(x-1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
  }
  position = (y-1)*width+x;
  if ((seed_array[position] == 0) && (aux_seed_array[position] != INSSL)) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,(y-1),x,seed_label,height,width);
    aux_seed_array[position] = INSSL; 
  }
  position = (y-1)*width+x+1;
  if ((seed_array[position] == 0) && (aux_seed_array[position] != INSSL)) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,(y-1),(x+1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
  }
  position = y*width+x+1;
  if ((seed_array[position] == 0) && (aux_seed_array[position] != INSSL)) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,y,(x+1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
  }
  position = (y+1)*width+x+1;
  if ((seed_array[position] == 0) && (aux_seed_array[position] != INSSL)) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,(y+1),(x+1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
  }
  position = (y+1)*width+x;
  if ((seed_array[position] == 0) && (aux_seed_array[position] != INSSL)) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,(y+1),x,seed_label,height,width);
    aux_seed_array[position] = INSSL;
  }
  position = (y+1)*width+x-1;
  if ((seed_array[position] == 0) && (aux_seed_array[position] != INSSL)) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,(y+1),(x-1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
  }
  position = y*width+x-1;
  if ((seed_array[position] == 0) && (aux_seed_array[position] != INSSL)) { 
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,y,(x-1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
  }
  
}


void add_to_region(int *seed_array, unsigned char *boundary_array, float *mean_averages, 
           unsigned char *image_array, 
           float delta_z, int seed, int no_of_seeds,
           int y, int x, int height, int width)

{
 int label,seed_label,position,neighbor_position,flag;
 float new_delta_z,right_mean,delta;
 
 flag =0;
 position = y*width+x;
 delta = delta_z;
 seed_label = seed; 

 neighbor_position = (y-1)*width+x-1;
 if ((seed_array[neighbor_position] !=0) && (seed_array[neighbor_position] != seed)) {
     label =  seed_array[neighbor_position];
     right_mean = mean_averages[label*3-3];
     new_delta_z = fabs((float)image_array[position]-right_mean);
     if (new_delta_z < delta) {
       seed_label = label;
       delta = new_delta_z;
       flag = 1;
     }
   }

 neighbor_position = (y-1)*width+x;
 if ((seed_array[neighbor_position] !=0 ) && (seed_array[neighbor_position] != seed)) {
     label = (int) seed_array[neighbor_position];
     right_mean = mean_averages[label*3-3];
     new_delta_z = fabs((float)image_array[position]-right_mean);
     if (new_delta_z < delta) {
       seed_label = label;
       delta = new_delta_z;
       flag = 1;
     }
   }

 neighbor_position = (y-1)*width+x+1;
 if ((seed_array[neighbor_position] !=0) && (seed_array[neighbor_position] != seed)) {
     label = (int) seed_array[neighbor_position];
     right_mean = mean_averages[label*3-3];
     new_delta_z = fabs((float)image_array[position]-right_mean);
     if (new_delta_z < delta) {
       seed_label = label;
       delta = new_delta_z;
       flag = 1;
     }
   }

 neighbor_position = y*width+x+1;
 if ((seed_array[neighbor_position] !=0) && (seed_array[neighbor_position] != seed)) {
     label = (int) seed_array[neighbor_position];
     right_mean = mean_averages[label*3-3];
     new_delta_z = fabs((float)image_array[position]-right_mean);
     if (new_delta_z < delta) {
       seed_label = label;
       delta = new_delta_z;
       flag = 1;
     }
   }

 neighbor_position = (y+1)*width+x+1;
 if ((seed_array[neighbor_position] !=0) && (seed_array[neighbor_position] != seed)) {
     label = (int) seed_array[neighbor_position];
     right_mean = mean_averages[label*3-3];
     new_delta_z = fabs((float)image_array[position]-right_mean);
     if (new_delta_z < delta) {
       seed_label = label;
       delta = new_delta_z;
       flag = 1;
     }
   }
 neighbor_position = (y+1)*width+x;
 if ((seed_array[neighbor_position] !=0) && (seed_array[neighbor_position] != seed)) {
      label = (int) seed_array[neighbor_position];
      right_mean = mean_averages[label*3-3];
      new_delta_z = fabs((float)image_array[position]-right_mean);
      if (new_delta_z < delta) {
    seed_label = label;
    delta = new_delta_z;
    flag = 1;
      }
    }
  neighbor_position = (y+1)*width+x-1;
  if ((seed_array[neighbor_position] !=0) && (seed_array[neighbor_position] != seed)) {
      label = (int) seed_array[neighbor_position];
      right_mean = mean_averages[label*3-3];
      new_delta_z = fabs((float)image_array[position]-right_mean);
      if (new_delta_z < delta) {
    seed_label = label;
    delta = new_delta_z;
    flag = 1;
      }
     }
  neighbor_position = y*width+x-1;
  if ((seed_array[neighbor_position] !=0) && (seed_array[neighbor_position] != seed)) {
      label = (int) seed_array[neighbor_position];
      right_mean = mean_averages[label*3-3];
      new_delta_z = fabs((float)image_array[position]-right_mean);
      if (new_delta_z < delta) {
    seed_label = label;
    delta = new_delta_z;
    flag = 1;
      }
     }

 seed_array[position] = seed_label;
 if (flag == 1) boundary_array[position] = 0;
 find_new_mean(mean_averages, image_array, seed_label, position); 
}


void find_new_mean(float *mean_averages, unsigned char *image_array, 
           int seed_label, int position)

{ 
  float new_total;
  
  new_total = mean_averages[seed_label*3-2] + (float) image_array[position];
  mean_averages[seed_label*3-3] = new_total/ (float) (mean_averages[seed_label*3-1]+1.0);
  mean_averages[seed_label*3-2] = new_total;
  mean_averages[seed_label*3-1] = mean_averages[seed_label*3-1]+1.0; 

}     

#ifdef __cplusplus
}
#endif





