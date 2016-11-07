#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DISTANCE_TRANSFORM_H_
#define _DISTANCE_TRANSFORM_H_

#include <utils.h>

typedef struct {
  float *dx, *dy;
  float *surface;
  int *dir, *region;
  unsigned char *tag;
}Cedt;

void signed_distance_transform (unsigned char *input_image, float *surface_array, 
                                int height, int width);

/****************************************************************************/

void dist_trans_by_cedt(float *surface, float band_size, int height, int width); 
void speed_expansion_by_cedt(Heap *heap, Cedt *cedt, float *level, float *speed, 
                             float *speed_exp, float band_size, int height, int width);

/**********************************************************************************/

void reinitialize_surface_by_fm(float *surface, float *level, Heap *heap, float band_size,
                                int height, int width);
void fast_marching(float *surface, float *tag_array, int band_size, Heap *heap,
                   int height, int width);



#endif /* DISTANCE_TRANSFORM */

#ifdef __cplusplus
}
#endif

