#include <stdio.h>
#include <math.h>
#include <utils.h>
#include <image_processing.h>

#include "region_utils.h"
#include "region.h"

int new_region(int *region_array,int y,int x,int *y_new, int *x_new, int height,int width)
     
{
  int xx,yy,pos,label,neighbor_label;
  
  label = region_array[y*width+x];
  
  for(yy=y-1; yy<(y+2); yy++) {
    for(xx=x-1; xx<(x+2); xx++) {
      pos = yy*width+xx;
      neighbor_label = region_array[pos];
      if (neighbor_label != BACKGROUND && neighbor_label != label) {
        *y_new = yy;
        *x_new = xx;
        return(neighbor_label);
      }
    }
  }
  
  *y_new = y;
  *x_new = x;
  return(label);
   
   
}


float find_new_image_value(float *image_array, int *region_array, float *normal_array, 
                            int y , int x, int height, int width)
     
{
  int xx,yy,pos,label,neighbor_label;
  float new_image_value;

  label = region_array[y*width+x];

  for(yy=y-1; yy<(y+2); yy++) {
    for(xx=x-1; xx<(x+2); xx++) {
      pos = yy*width+xx;
      if (neighbor_label == BACKGROUND || neighbor_label != label) {
        return(image_array[pos]);
      }
    }
  }
  
  /* look at two two pixel away in normal direction */
  yy = normal_outward_direction(y,x,normal_array,1,2,height,width);
  xx = normal_outward_direction(y,x,normal_array,2,2,height,width);
  new_image_value = image_array[yy*width+xx];
  return(new_image_value);
   
}


int check_merging_procedure(float mean,float stdev,float new_mean,float new_stdev)
     
{
  if ((fabs(new_mean-mean) < 1.0*stdev) && (fabs(new_mean-mean) < 1.0*new_stdev))
    return(1);
   else return(0);
}



void merge_regions(BubStatistics *bubst, int *region, float *image, int label, int new_label, 
                   int height, int width)

{
  int y,x,pos;
  float imagevalue;

#ifdef TEST  
  printf("merging regions: \n");
  printf("   region = %i with mean=%lf and stdev=%lf and N=%i\n", label,
         bubst->mean[label],bubst->stdev[label],bubst->n[label]);
   printf("   region = %i with mean=%lf and stdev=%lf and N=%i\n", new_label,
         bubst->mean[new_label],bubst->stdev[new_label],bubst->n[new_label]);
#endif TEST  

   for(y=0; y<height; y++) {
     for(x=0; x<width; x++) {
       pos = y*width+x;
       if (region[pos] == new_label) {
         region[pos] = label;
         imagevalue = image[pos];
         find_new_mean_and_stdev(bubst,label,imagevalue); 
       }
     }
   }
   
   bubst->mean[new_label] = bubst->mean[label];
   bubst->stdev[new_label] = bubst->stdev[label];
   bubst->n[new_label] = bubst->n[label];

#ifdef TEST   
   printf("and now regions have been merged: \n");
   printf("   region = %i with mean=%lf and stdev=%lf and N=%i\n", label,
          bubst->mean[label],bubst->stdev[label],bubst->n[label]);
   printf("   region = %i with mean=%lf and stdev=%lf and N=%i\n", new_label,
          bubst->mean[new_label],bubst->stdev[new_label],bubst->n[new_label]);
#endif TEST   
}








