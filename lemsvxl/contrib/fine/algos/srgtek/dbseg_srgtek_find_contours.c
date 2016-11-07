#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<math.h>
#include "dbseg_srgtek_find_contours.h"
#include "dbseg_srgtek_contour_routines.h"


void find_all_contours(int *image_array, unsigned char *boundary_array, int no_of_seeds, 
               int height, int width)

{
  int *temp_array;
  int y,x,pos,pos1,i;
  FILE *fopen(), *f1;
  int length_of_contour;
  length_of_contour = height*width;
  
  temp_array = (int *) malloc(height*width*sizeof(int));
  
  pos=0;
  for (y=0; y<height; y++){
    for(x=0; x<width; x++) {
      boundary_array[pos] = 255;
      pos++;
    }
  }

  for(i=2; i<=no_of_seeds; i++) {
    find_start(i, temp_array, length_of_contour, image_array, (width-1), (height-1));
    pos=0; pos1=0;
    while(temp_array[pos] != ENDOFCHAINLIST) {
      while (temp_array[pos] != CHAINEND) {
           x = temp_array[pos]; pos++;
           y = temp_array[pos]; pos++; 
           boundary_array[y*width+x] = 0;
      }
    pos++;
   }
}

  free(temp_array);
}
     
/*Function that calculates the mean value of the image for each seed*/

void find_mean_averages(int *seed_array, unsigned char *image, float *mean_averages, 
            int no_of_seeds, int height, int width)

{ 
  int y,x,pos,pos1,value,seeds,number_of_points;
  float total;
  float temp;
  float std_dev;

  /*Loop to calculate the mean averages of the seeds*/
  pos1=0;
  for(seeds =1; seeds<=no_of_seeds; seeds++) {
    pos=0; total=0.0; number_of_points=0; 
    for(y=0; y<height; y++) {
      for(x=0; x<width; x++) {
        value = seed_array[pos];
        /*    printf("%d ",value);*/
        if (value == seeds) {
            total = total + (float) image[pos];
            temp = (float) image[pos];
            /*printf("T=%f  I=%f \n", total,temp);*/
            number_of_points++;
        }
         pos++;
      }
     }
    
    /*The loop to calculate the standard deviation of the region
      under the seeds*/

        mean_averages[pos1] = (float) (total/(float) number_of_points); pos1++;
        printf("%f ",mean_averages[pos1-1]);
        mean_averages[pos1] = (float) total; pos1++;
        printf("%f ",(float) total);
        mean_averages[pos1] = (float) number_of_points; pos1++;
     printf("%f \n",(float) number_of_points);
  }
    pos1=0;
    std_dev = 0;
    for(seeds =1; seeds<=no_of_seeds; seeds++) {
      pos=0; total=0.0; number_of_points=0; 
      for(y=0; y<height; y++) {
        for(x=0; x<width; x++) {
            if (seed_array[pos] == seeds) {
                std_dev = std_dev + (float) image[pos]-mean_averages[pos1];    
            }
        }
      }
    }
  
}
    

void initial_neighbors(int *contour_array, int *seed_array, 
               unsigned char *boundary_array,
               int *aux_seed_array, unsigned char *image_array, 
               float *mean_averages, float *ssl_array, 
               int no_of_seeeds, int height, int width)

{
  int y,x,pos,position,seed_label;
  float right_mean,delta_z;

  pos=0;
  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      if ((y==0) || (y==(height-1)) || (x==0) || (x==(width-1))) {
    aux_seed_array[pos] = INSSL; /* -1 */
      }
      else aux_seed_array[pos] = seed_array[pos];
      boundary_array[pos] = 255;
      pos++;
    }
  }
  
  /*  CHAINEND -1
     ENDOFCHAINLIST -2*/
  pos=0; 
  while(contour_array[pos] != ENDOFCHAINLIST) {
    while (contour_array[pos] != CHAINEND) {
      x = contour_array[pos]; pos++;
      y = contour_array[pos]; pos++;
      seed_label = seed_array[y*width+x];
      right_mean = mean_averages[seed_label*3-3]; 

      position = (y-1)*width+x-1;
      if (seed_array[position] == 0) {
    delta_z = fabs((float)image_array[position]-right_mean);
    put_into_ssl(delta_z,ssl_array,(y-1),(x-1),seed_label,height,width);
    aux_seed_array[position] = INSSL; 
      }
      position = (y-1)*width+x;
      if (seed_array[position] == 0) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,(y-1),x,seed_label,height,width);
    aux_seed_array[position] = INSSL;
      }
      position = (y-1)*width+x+1;
      if (seed_array[position] == 0) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,(y-1),(x+1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
      }
      position = y*width+x+1;
      if (seed_array[position] == 0) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,y,(x+1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
      }
      position = (y+1)*width+x+1;
      if (seed_array[position] == 0) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,(y+1),(x+1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
      }
      position = (y+1)*width+x;
      if (seed_array[position] == 0) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,(y+1),x,seed_label,height,width);
    aux_seed_array[position] = INSSL;
      }
      position = (y+1)*width+x-1;
      if (seed_array[position] == 0) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,(y+1),(x-1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
      }
      position = y*width+x-1;
      if (seed_array[position] == 0) {
    delta_z = fabs((float)image_array[position]-right_mean); 
    put_into_ssl(delta_z,ssl_array,y,(x-1),seed_label,height,width);
    aux_seed_array[position] = INSSL;
      }
    }
    pos++; 
  }

}



void put_into_ssl(float delta_z, float *ssl_array, int y, int x, int seed_label,
          int height, int width)

{
  int i,pos,position,flag;
  float value;

  pos=0; flag =0;
  while(ssl_array[pos] != END) {
    value = ssl_array[pos];
    if ((flag == 0) && (delta_z <= value)) {
      position = pos;
      flag =1;
    }
   pos++; pos++; pos++; pos++;
  }

  for(i=pos; i>=position; i--) {
    ssl_array[i+4] = ssl_array[i];
  }
  ssl_array[position] = delta_z; position++;
  ssl_array[position] = (float) y; position++;
  ssl_array[position] = (float) x; position++;
  ssl_array[position] = (float) seed_label; 
}




  

      
       
  
  

#ifdef __cplusplus
}
#endif





