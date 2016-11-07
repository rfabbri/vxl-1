#ifdef __cplusplus
extern "C" {
#endif

#ifndef _find_contours_h_
#define _find_contours_h_

#define INSSL -10
#define END -1.0

void find_all_contours(int *image_array, unsigned char *boundary_array,int no_of_seeds,
               int height, int width);
void find_mean_averages(int *seed_array, unsigned char *image, float *mean_averages, 
            int no_of_seeds, int height, int width);
void initial_neighbors(int *contour_array, int *seed_array, unsigned char *boundary_array, 
               int *aux_seed_array,unsigned char *image_array, 
               float *mean_averages, float *ssl_array, int no_of_seeeds,
               int height, int width);
void put_into_ssl(float delta_z, float *ssl_array, int y,int x, int seed_label,
          int height, int width);

#endif

#ifdef __cplusplus
}
#endif

