#ifndef _SAVE_2D_BUBBLES_RESULTS_H_
#define _SAVE_2D_BUBBLES_RESULTS_H_

void save_2d_ED_bubbles_results(char *outimage, float *surface_array, float *level_set_array, 
                             float alpha, float beta, int how_often_save,int save_surface,
                             int iteration, float band_size, int height, int width);

void save_2d_RD_bubbles_results(char *outimage, float *surface_array, float *level_set_array, 
                                int *region, float alpha, float beta, int how_often_save,
                                int save_surface,int iteration, float band_size, 
                                int height, int width);



#endif /* SAVE_2D_BUBBLES_RESULTS */


