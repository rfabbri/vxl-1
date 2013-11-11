#include <stdio.h>
#include <math.h>
#include <utils.h>
#include <image_processing.h>

#include "region.h"
#include "region_utils.h"
#include "region_stop.h"


void region_stop_function (BubStatistics *bubst, float *surface, float *level,
                           float *stop_array, int *region,float *image,
                           float stop_function, float band_size, int height, int width)
     
{
  int x_pos[4] = {1,1,0,-1};
  int y_pos[4] = {0,1,1,1};
  int y,x,yy,xx,pos,label,new_label,N,N_new,merge;
  int ii,pos2,new_position;
  float mean,new_mean,stdev,new_stdev,temp_value,stop_value;
  float image_value,new_image_value,level_set_value;
  int position,position1,position2,position3,flag;
  float interpolation,point_value,neighbors_value,zero_crossing;
  int labels[3];
  char out[128] = "";
  float temp1,temp2,stop1,stop2;
  FILE *fopen(), *f1;

  
  update_statistics_from_level_set (surface,region,image,bubst, band_size, height, width);
  
  
  flag =0;
  InitializeImage_Float(level, 255.0, height, width);
  InitializeImage_Float(stop_array, 1.0, height, width);


  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {
      position = y*width+x;
      if (fabs(surface[position]) <= band_size) {
        point_value =  surface[position];
        for(ii=0; ii<4; ii++) {
          new_position = (y+y_pos[ii])*width +(x+x_pos[ii]);
          neighbors_value = surface[new_position];
          zero_crossing = point_value*neighbors_value;
          if (zero_crossing <= 0.0) {
            if (point_value > 0.0) {
              level[position] = region[position];
              level[new_position] = -region[position];
            }
            else {
              level[new_position] = region[new_position];
              level[position] = -region[new_position];
            }
          }
        }
      }
    }
  }
  
  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {
      pos = y*width+x;
      merge = 1.0;
      if (level[pos] < 0.0) {
        ii =1; labels[0] = -level[pos]; 
        for(yy=y-1; yy<(y+2); yy+=2) {
          for(xx=x-1; xx<(x+2); xx+=2) {
            pos2 = yy*width+xx;
            if (level[pos2] < 0.0 && level[pos2] != -labels[ii-1]) {
              labels[ii] = -level[pos2];
              ii++;
            }
          }
        } 
        mean = bubst->mean[labels[0]];
        stdev = bubst->stdev[labels[0]];
        if (stdev < 1.0) 
          stdev = 1.0;
        image_value = image[pos];
        temp_value = 1.0+(pow(fabs(image_value - mean),stop_function)/stdev);
        stop_value = (1.0/temp_value); 
        if (ii > 1) {
          image_value = image[pos];
          mean = bubst->mean[labels[0]];
          stdev = bubst->stdev[labels[0]];
          if (stdev < 1.0) stdev = 1.0;
          new_mean = bubst->mean[labels[1]];
          new_stdev = bubst->stdev[labels[1]];
          if (new_stdev < 1.0) new_stdev = 1.0;
          merge = check_merging_procedure(mean,stdev,new_mean,new_stdev);
          if (merge == 1.0) {
            merge_regions(bubst, region, image, labels[0],labels[1], height, width);
          }
          else {
#ifdef TEST
            temp1 = 1.0/(1.0+(pow(fabs(image_value - mean),stop_function)/stdev));
            temp2 = 1.0/(1.0+(pow(fabs(image_value - new_mean),stop_function)/new_stdev));
            stop1 = temp1-temp2;
            stop2 = temp2-temp1;
            for(yy=y-1; yy<(y+2); yy+=2) {
              for(xx=x-1; xx<(x+2); xx+=2) {
                pos2 = yy*width+xx;
                if (level[pos2] < 0.0) {
                  if (level[pos2] == -labels[0]) {
                    stop_array[pos2] = stop1;
                  }
                  else if (level[pos2] == -labels[1]) {
                    stop_array[pos2] = stop2;
                   }
                }
              }
            }
#endif TEST
          }
        }
        if ((stop_value < 1.1482101838810883E-10) && (stop_value > 0.0))
          stop_value = 0.0;
        if (merge == 1) 
          stop_array[pos] = stop_value;
        else stop_array[pos] = 0.0; 
      }
    }
  }
  
  adjust_boundaries_float(stop_array, height, width); 

  for(y=1; y<(height-1); y++) {
    for(x=1; x<(width-1); x++) {
      pos = y*width+x;
      if (level[pos] < 0.0) 
        level[pos] = 0.0;
      else 
        level[pos] = 255.0;
    } 
  }

}









