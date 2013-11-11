#ifdef __cplusplus
extern "C" {
#endif

#ifndef _IMAGE_PROCESSING_H_
#define _IMAGE_PROCESSING_H_


/*********************  image_statistics.h ********************/
void compute_stdev_in_a_window (double *image, double *deviation, double size,
                                double smoothing, int height, int width);

/****************** curvature_smoothing.h **************************/
void curvature_smoothing(double *image_array, double beta, int steps, int scale, 
                         int height, int width);

void compute_diffusion_flux (double *surface_array, double *diffusion_array,
                             int height, int width);
    
/****************** constant_deformation.h ********************/
void constant_deformation(double *image_array, double alpha, int steps, int height, int width);
double hamilton_jacobi_flux (double Dxm, double Dxp, double Dym, double Dyp);
void get_reaction_flux_values (double *surface_array,double *reaction_array, 
                               double alpha, double delta_x,
                               double delta_y,int height, int width);
/********************** normal.h ******************************/
int normal_outward_direction(int y,int x,double *normal_array,int y_or_x,int length_of_vector,
                 int height, int width);
int normal_inward_direction(int y,int x, double *normal_array,int y_or_x,int length_of_vector,
                int height, int width);
int normal_vector_x_3 (double angle);
int normal_vector_y_3 (double angle);
int normal_vector_x_2 (double angle);
int normal_vector_y_2 (double angle);
int normal_vector_x_1 (double angle);
int normal_vector_y_1 (double angle);


#endif /* IMAGE_PROCESSING */

#ifdef __cplusplus
}
#endif

