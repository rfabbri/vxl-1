#ifndef _INITAILIZE_DEFORM_H_
#define _INITAILIZE_DEFORM_H_

void initialize_deform(Bubble_Deform *BubDeform, float *image_array, float *stop_array, 
                       float *surface_array, int edge_driven, float alpha, float beta,
               float gamma, float delta_y,float delta_x,float band_size,
                       int height, int width);

#endif /* INITAILIZE_REGIONS */


