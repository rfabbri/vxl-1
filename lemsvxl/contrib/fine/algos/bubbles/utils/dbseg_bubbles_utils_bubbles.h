#ifndef _BUBBLES_H_
#define _BUBBLES_H_
#include <distance_transform.h>

#define BACKGROUND -1.0
#define ENDOFNBAND -10
#define ENDOFOEDGES -20
#define ENDOFIEDGES -20
#define NBAND 0

typedef struct {
  float *stdev,*mean;
  float *sumisq;
  int *n;
  int number_of_region;
}Region_Statistics;

typedef struct {
  int height, width,iteration_number;
  float alpha,beta,gamma;
  float *surface_array;
  unsigned char *seeds_array;
  float *level_set_array;
  int *region_array;
  Region_Statistics region_statistics;
}Bubbles;

typedef struct {
  int height, width;
  float alpha,beta,gamma;
  float band_size;
  float delta_x,delta_y,delta_t;
  float *image_array;
  float *stop_array;
  int *band_array;
  Bubbles bubbles;
  Cedt cedt;
  Heap heap;
  float *reaction_array, *diffusion_array, *stop_exp_array;
}Bubble_Deform;


/****       deform_ED_bubbles.c *********************/
void deform_ED_bubbles(Bubble_Deform *BubDeform, int iteration);


/************************* bubble_deform.h *********************/
void initialize_deform(Bubble_Deform *BubDeform, float *image_array, float *stop_array, 
                       float *surface_array, int edge_driven, float alpha, float beta,
               float gamma, float delta_y,float delta_x, float band_size,
                       int height, int width);

/**************** edge_driven_stop_values ***************/
void edge_driven_stop_values(float *image_array,float *stop_array,float k,
                             int height,int width);

/*************** initialize_bubbles.c *****************/
void random_bubbles_in_uniform_regions(float *image_array, unsigned char *bubbles_array, 
                                       int *intensity_intervals, float stdev,float window_size,
                                       int height, int width);     

void initialize_bubbles(float *image_array, unsigned char *bubbles_array,  
                        int *intensity_intervals, float stdev, float window_size,
                        int userinit, int height, int width); 


/************** flux.h *********************/
float hamilton_jacobi_flux (float u_l, float u_r, float u_u, float u_d);
void reaction_flux_values (float *surface_array, float *reaction_array, int band_size, 
                           float flow_direction, int height, int width);
void diffusion_flux_values (float *surface_array, float *diffusion_array,
                            int band_size, int height, int width);    

/********************** narrow_band.c ***********************/
void narrow_band_boundaries(float *surface_array, float *level_array, int *nband_edges, 
                            float band_size, int height, int width);
int zero_level_in_narrow_band(float *surface_array, int *nband_edges, float level,
                              int height, int width);

/************************ save results **********************************/
void save_2d_ED_bubbles_results(char *outimage, float *surface_array, float *level_set_array, 
                                float alpha, float beta, int how_often_save,int save_surface,
                                int iteration, float band_size, int height, int width);

void save_2d_RD_bubbles_results(char *outimage, float *surface_array, float *level_set_array, 
                                int *region, float alpha, float beta, int how_often_save,
                                int save_surface,int iteration, float band_size, 
                                int height, int width);

/******************  fill_bubbles.c *******************************/

int fill_bubbles(float *surface, int *region, int boundary, int height, int width);


/********************** image_statistics.h ***********************************/
void bubbles_mean_and_stdev (Region_Statistics *bubst, int *region, float *image, 
                             int height, int width);

#endif /* BUBBLES */


