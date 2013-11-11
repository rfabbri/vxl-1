#ifndef _region_h_
#define _region_h_

#include "bubbles_region.h"

void init_bubbles_mean_and_stdev (BubStatistics *bubst, int *region, float *image,  
                                 int height, int width);
void find_new_mean_and_stdev(BubStatistics *bubst, int label, float imagevalue);
void update_statistics_from_level_set (float *surface_array,int *region, float *image, 
                                       BubStatistics *bubst, float band_size,
                                       int height, int width); 



#endif /* _region_h_ */



