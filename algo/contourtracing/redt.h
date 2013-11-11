#ifndef _REDT_H_
#define _REDT_H_

void signed_distance_transform (unsigned char *input_image, float *surface_array,
                                int height, int width);

void euclidean(unsigned char *image_array,float *dt_array,int height,int width,int direction); 
void create_distance_images(unsigned char *image, float *dt_x,float *dt_y,
                            int height, int width, int distance_from_interior);
void compute_euclidean_distance_array_squared(float *array_x, float *array_y,
                                              float *array,int height, int width);

 

#endif /* REDT */
