#ifdef __cplusplus
extern "C" {
#endif

#ifndef _BUBBLES_H_
#define _BUBBLES_H_
/* #include <distance_transform.h>*/
#include <cedt.h>

#define BACK_GROUND -1.0
#define ENDOFNBAND -10
#define ENDOFOEDGES -20
#define ENDOFIEDGES -20
#define NBAND 0

typedef struct {
  double *stdev,*mean;
  double *sumisq;
  int *n;
  int number_of_region;
}Region_Statistics;

typedef struct {
  int height, width,iteration_number;
  double alpha,beta,gamma;
  double *surface_array;
  unsigned char *seeds_array;
  double *level_set_array;
  int *region_array;
  Region_Statistics region_statistics;
}Bubbles;

typedef struct {
  int height, width;
  double alpha,beta,gamma;
  double band_size;
  double delta_x,delta_y,delta_t;
  double *image_array;
  double *stop_array, *stop_exp_array;
  double *stopx_array, *stopx_exp_array;
  double *stopy_array, *stopy_exp_array;
  int *band_array;
  Bubbles bubbles;
  Cedt cedt;
  Heap heap;
  double *reaction_array, *diffusion_array;
}Bubble_Deform;


/****       deform_ED_bubbles.c *********************/
void deform_ED_bubbles(Bubble_Deform *BubDeform, int iteration);


/************************* bubble_deform.h *********************/
void initialize_deform(Bubble_Deform *BubDeform, double *image_array, double *stop_array, 
                       double *surface_array, int edge_driven, double alpha, double beta,
               double gamma, double delta_y,double delta_x, double band_size,
                       int height, int width);

void free_deform(Bubble_Deform *BubDeform, int edge_driven);


/**************** edge_driven_stop_values ***************/
void edge_driven_stop_values(double *image_array,double *stop_array,double k,
                             int height,int width);

void compute_stop_derivs(double *stop_array,double *stopx_array,double *stopy_array,
             int height,int width);
/*************** initialize_bubbles.c *****************/

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

void random_bubbles_in_uniform_regions(double *image_array, unsigned char *bubbles_array, 
                                       int *intensity_intervals, double stdev,double window_size,
                                       int height, int width);     

void initialize_bubbles(double *image_array, unsigned char *bubbles_array, int *seeds, 
                        int *intensity_intervals, double stdev, double window_size,
                        int userinit, int height, int width);

double rand0(int *idum);

/************** flux.h *********************/
double hamilton_jacobi_flux (double u_l, double u_r, double u_u, double u_d);
void reaction_flux_values (double *surface_array, double *reaction_array, int band_size, 
                           double flow_direction, int height, int width);
void diffusion_flux_values (double *surface_array, double *diffusion_array,
                            int band_size, int height, int width);    

/************** geodesic-flux.h *********************/

double upwind_derivatives (double u_l, double u_r);

void compute_geodesic_flux(double *surface_array, double *stopx_array, double *stopy_array,
               int height, int width);

/********************** narrow_band.c ***********************/
void narrow_band_boundaries(double *surface_array, double *level_array, int *nband_edges, 
                            double band_size, int height, int width);
int zero_level_in_narrow_band(double *surface_array, int *nband_edges, double level,
                              int height, int width);

/************************ save results **********************************/
void save_2d_ED_bubbles_results(char *outimage, double *surface_array, double *level_set_array, 
                                double alpha, double beta, int how_often_save,int save_surface,
                                int iteration, double band_size, int height, int width);

void save_2d_RD_bubbles_results(char *outimage, double *surface_array, double *level_set_array, 
                                int *region, double alpha, double beta, int how_often_save,
                                int save_surface,int iteration, double band_size, 
                                int height, int width);

/******************  fill_bubbles.c *******************************/

int fill_bubbles(double *surface, int *region, int boundary, int height, int width);


/********************** image_statistics.h ***********************************/
void bubbles_mean_and_stdev (Region_Statistics *bubst, int *region, double *image, 
                             int height, int width);

#endif /* BUBBLES */

#ifdef __cplusplus
}
#endif

