#ifndef _region_utils_h_
#define _region_utils_h_

#include "bubbles_region.h"

int new_region(int *region_array,int y,int x,int *y_new, int *x_new, int height,int width);
float find_new_image_value(float *image_array, int *region_array, float *normal_array, 
                            int y , int x, int height, int width);
int check_merging_procedure(float mean,float stdev,float new_mean,float new_stdev);
void merge_regions(BubStatistics *bubst, int *region, float *image, int label, int new_label, 
                   int height, int width);



#endif /* _region_h_ */



