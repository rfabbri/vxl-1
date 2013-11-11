#ifndef _BUBBLES_UTILS_H_
#define _BUBBLES_UTILS_H_



/**************** edge_driven_stop_values ***************/
void edge_driven_stop_values(float *image_array,float *stop_array,float k,
                             int height,int width);

/*************** initialize_bubbles.c *****************/
void initialize_bubbles(float *image_array, float *bubbles_array, int *seeds, int userinit,
                        float threshold, float circle_size, 
                        int initialize_min, int initialize_max, int height, int width); 

/************** flux.h *********************/
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
void save_2d_bubbles_results(char *outimage, float *surface_array, float *level_set_array, 
                             float alpha, float beta, int how_often_save,int save_surface,
                             int iteration, float band_size, int height, int width);

#endif /* BUBBLES_UTILS */


