#include <stdio.h>
#include <vcl_cmath.h>
#include "utils.h"

#include "signed_dt.h"
#include "euclidean_dist.h"


void signed_distance_transform (unsigned char *input_image, float *surface_array,
                                int height, int width)

{
  int y,x,position,inside_region;
  int outward;
  float *aux_dt_array, surface_value,diff, difference,cutoff_margin,
          distance_from_interior,distance_from_exterior;
  FILE *fopen(),*f1;
  cutoff_margin = 1000.0;
  outward = 1;

  aux_dt_array = (float *) malloc (height*width*sizeof(float));

  printf("in signed dt\n");
  printf("height =%i and width =%i\n", height,width);

  euclidean(input_image,surface_array,height,width,!outward);

  euclidean(input_image, aux_dt_array,height,width,!!outward);

  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      position = width*y + x;

      if (surface_array[position] >= 1.0)
        distance_from_interior = sqrt(surface_array[position]-1.0);

      else distance_from_interior = 0.0;
      distance_from_exterior = sqrt(aux_dt_array[position]);

      if (input_image[position] != 1.0)  inside_region =1;
      else inside_region = 0;

      if (((outward ==1) && (inside_region ==1)) ||
          ((outward ==0) && (inside_region == 0))) {
        difference = cutoff_margin - distance_from_interior;
        if (difference > 0.0) diff = difference;
        else diff = 0.0;
        surface_value = diff - cutoff_margin;
      }
      else surface_value = distance_from_exterior;

      surface_array[position] =  surface_value-0.5;

    } /* for x=0....  */
  } /* for y=0..    */

  free((char *) aux_dt_array);
}




