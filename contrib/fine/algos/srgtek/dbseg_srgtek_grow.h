#ifdef __cplusplus
extern "C" {
#endif

#ifndef _grow_h_
#define _grow_h_

void grow_regions(int *seed_array, unsigned char *boundary_array,int *aux_seed_array,
          unsigned char *image_array, float *ssl_array, int no_of_seeds,
          float *mean_averages, int height, int width);
void pull_from_ssl(float *ssl_array);
void put_neighbors_into_ssl(float *ssl_array, float *mean_averages, int *seed_array, 
                int *aux_seed_array, unsigned char *image_array,
                int seed_label, int y, int x, int height, int width);
void add_to_region(int *seed_array, unsigned char *boundary_array,
           float *mean_averages, unsigned char *image_array, 
           float delta_z, int seed, int no_of_seeds,
           int y, int x, int height, int width);
void find_new_mean(float *mean_averages, unsigned char *image_array, 
           int seed_label, int position);



#endif





#ifdef __cplusplus
}
#endif

