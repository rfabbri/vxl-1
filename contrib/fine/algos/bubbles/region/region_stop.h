#ifndef _region_stop_h_
#define _region_stop_h_

#include "bubbles_region.h"

void region_stop_function(Region_Statistics *bubst, float *surface,float *level,
                          float *stop_array, int *region,float *image, 
                          float stop_function, float band_size, int height, int width);
#endif 


