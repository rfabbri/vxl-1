#include <math.h>
#include <stdio.h>

#include <utils.h>
#include <bubbles.h>

#include "region.h"


void initialize_regions(Bubbles *bubbles, BubStatistics *bubst, int height, int width) 

{
 
  bubst->region_number = fill_bubbles(bubbles->surface_array, bubbles->region,0,height,width);
  init_bubbles_mean_and_stdev (bubst, bubbles->region, bubbles->image_array,height,width);

}








