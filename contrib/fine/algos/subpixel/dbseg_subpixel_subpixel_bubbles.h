#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SUBPIXEL_BUBBLES_H_
#define _SUBPIXEL_BUBBLES_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "dbseg_subpixel_utils.h"
#include "dbseg_subpixel_image_processing.h"
#include "dbseg_subpixel_tbs-utils.h"
#include "dbseg_subpixel_graphics.h"
#include "dbseg_subpixel_geno-types.h"
#include "dbseg_subpixel_geno.h"
#include "dbseg_subpixel_cedt.h"
#include "dbseg_subpixel_redt.h"
#include "dbseg_subpixel_1d_eno_types.h"
#include "dbseg_subpixel_non-uniform-eno.h"
#include "dbseg_subpixel_postscript.h"
#include "dbseg_subpixel_SPCedtGeno.h"
#include "dbseg_subpixel_boundary_from_geno.h"


//Eli added this to hopefully make it compile
#define M_PI 3.141592653

#define PAD 2

#define DISTANCE_DROP_THRESHOLD 0.3 /* Distance below which distance
                    coupling is 1*/
#define DISTANCE_DROP_SIGMA 1.0 /*Sigma used for distance coupling drop off*/
#define MAX_SHOCK_DISTANCE 3 /* ~3 times the DISTANCE_DROP_SIGMA. Max.
                distance from the shock to boundary for 
                boundary to be considered for coupling*/

#define END_POINT_TOLERANCE 0.15 /* Tolerance to look at adjacent intervals
                    if boundary point closest to a grid is
                    near the end point*/
#define MAX_NO_REGIONS 600
#define MAX_BOUNDARY_LENGTH 15000
#define MAX_SHOCK_LENGTH 3*MAX_BOUNDARY_LENGTH

typedef struct {
  double alpha;
  double beta;
  double gamma;
  double band_size;
  double delta_x,delta_y;
  double delta_t;
  int total_no_iterations;
  int start_from;
  int save_levelset, save_surface;
  int iteration_no;
  int height,width;
  int pad;
  char outbase[200];
  char region_file[200];
  char line_file[200];
  int line_flag;
  int region_flag;
  double gauss_smooth;
  int curv_smooth;
  int no_of_classes;
}SP_Bubbles_Params;


typedef struct {
  int region_label;
  double sum;
  double sum_sq;
  double mean;
  double stdev;
  double mean_vec[2];
  double std_vec[2];
  double alpha_vec[2];
  int no_of_classes;
  int no_of_pixels;
}Region_Stats;

typedef struct {
  /*int height,width;*/
  double *surface;
  int *region_label_array;
  double *zero_level_set;
  Region_Stats *region;
  int no_of_regions;
}SP_Bubbles;

typedef struct{
  double *snake_force;
  double *Sx;
  double *Sy;
}Snake_Force;

typedef struct {
  double *edge_force;
  double *line_force;
  Snake_Force snake;
  double *statistics_force;
  double *stop;
  int *force_direction;
  Cedt exp_cedt;
  Heap exp_heap;
}SP_Bubbles_Force;

typedef struct{
  double *mean;
  double *stdev;
  int *no_of_pixels;
}Window_Stats;


typedef struct {
  /*int height,width;*/
  double *image;
  double *x_image_grad,*y_image_grad;
  double *x_image_2grad,*y_image_2grad;
  double *xy_image_grad,*yx_image_grad;
  double *reaction, *diffusion;
  double *update;
  unsigned char *seed;
  SP_Bubbles bubbles;
  SP_Bubbles_Force force;
  SP_Bubbles_Params params;
  ENO_Interval_Image_t *enoimage;
  ENO_Interval_Image_t *enosurface;
  SPCedt cedt;
  CEDTWaves waves;
  Heap heap;
  Boundary *linear_boundary;
  Shocks shocks;
  int *back_shock_ptr;
  int num_shocks;
  Boundary *shock_boundary;
}SP_Bubble_Deform;


/**************************************************************************
*****************************region_utils.c********************************
**************************************************************************/

void find_mean_stdev_region(double *image_array, int *seed_array,  
                int region_flag, int height,   
                int width, Region_Stats *region);

void find_em_multi_mean_stdev_region(double *image_array, int *seed_array,  
                     int region_flag, int height,   
                     int width, Region_Stats *region);

double find_statistics_force(double intensity, Region_Stats *region,
                int region_flag,int no_of_regions);

double find_statistics_force_window(Window_Stats *win, int x, int y,
                   int height, int width, Region_Stats *region,
                   int region_flag,int no_of_regions);

void compute_square_window_stats(double *image_array,Window_Stats *win, 
                int height, int width, int window_size);

void print_region_stats(Region_Stats *region,FILE *fp);

void update_region_structure(SP_Bubble_Deform *spbub_deform);

int find_regions(int *region_label_array,int height,int width);

int update_region_stats(int *region_label_array, double *image, 
            Region_Stats *region,int no_of_classes, 
            int height, int width);

/**************************************************************************
****************************initialize_utils.c******************************
**************************************************************************/

void allocate_window_stats(Window_Stats *win, int height, int width);

void allocate_grid_cedt_data(Cedt *cedt, int height, int width);

void free_grid_cedt_data(Cedt *cedt);

void initialize_data_SP_Cedt(SPCedt *cedt, int height, int width);

void free_data_SP_Cedt(SPCedt *cedt);

void allocate_data_SP_Cedt(SPCedt *cedt, int height, int width);     

void allocate_sp_bubbles_force(SP_Bubbles_Force *force, int height, int width);

void allocate_sp_bubbles(SP_Bubbles *bubbles, int height, int width, 
             int no_of_regions);

void free_sp_bubbles(SP_Bubbles *bubbles);

void free_sp_bubbles_force(SP_Bubbles_Force *force);

void free_data_QWaves_list(QWaves *qwaves, int size);

void initial_wave_prop_cedt_strucs(Heap *heap, SPCedt *cedt, CEDTWaves *waves,
                   int height, int width);


/**************************************************************************
*******************************initialize.c********************************
**************************************************************************/

/*void initialize_sp_bub_deform_args(int argc, char *argv[],
                   SP_Bubble_Deform * spbub_deform);*/
void initialize_sp_bub_deform_args(double* imageR, unsigned char* seedR, int hei, int wid, int numitr,
                   SP_Bubble_Deform * spbub_deform);

void initialize_sp_bub_params(int pad, int iterations, double alpha, double beta, 
                  int height,int width,SP_Bubbles_Params *params);

void allocate_sp_bub_deform(SP_Bubble_Deform * spbub_deform,
                int height,int width,int reg_ct);

void free_sp_bub_deform(SP_Bubble_Deform * spbub_deform);

/**************************************************************************
*****************************read_parameters.c*****************************
**************************************************************************/
void initialize_sp_bub_params_args(SP_Bubbles_Params *params, int height, int width, int numitr);;
//void initialize_sp_bub_params_args(SP_Bubbles_Params *params, int argc, char *argv[]);

void usage();

void print_parameters(SP_Bubbles_Params *params);

void read_input_bubble_arrays(double *image, unsigned char *seed, double *surface,
                  int *region, char *argv[],SP_Bubbles_Params *params);


/**************************************************************************
*****************************normal_interp.c*******************************
**************************************************************************/
#define NU_ENO_MAXORDER 2
#define IMPOSSIBLE_VALUE -1000
double find_grid_gradient(Point boundary_point, ENO_Interval_Image_t *enoimage, 
             double *grad_x, double *grad_y);

void compute_subpixel_interp_ders(Point grid_point, Point boundary_point,
                  ENO_Interval_Image_t *enoimage, 
                  double *first_der,double *second_der,
                  double *interp_value);

int compute_points_for_interpolation(Point grid_point, Point boundary_point,
                     Point *interp_point,int N, int height,
                     int width);

/**************************************************************************
*******************************eno_utils.c*********************************
**************************************************************************/

double find_correct_eno_interpolation(ENO_Interval_Image_t *enoimage, Point pt);

double find_grid_eno_gradient(Point boundary_point, ENO_Interval_Image_t *enoimage, 
                 double *grad_x, double *grad_y);

double find_grid_eno_2nd_der(Point pt, ENO_Interval_Image_t *enoimage, 
                double *x_2der, double *y_2der);

/**************************************************************************
********************************curvature.c********************************
**************************************************************************/
#define MAX_CURVATURE 4.0

double compute_curvature_geno_boundary(Boundary *boundary, Point boundary_point,
                      Point grid_point,double *surface, 
                      int height,int width);



double compute_curvature_anti_geno_fit(Boundary *boundary, Point boundary_point, 
                       int boundary_id, Point grid_point,
                       double *surface, 
                       Region_Stats *region,int height,int width);

double compute_coupled_curvature(Boundary *boundary, Point boundary_point1, 
                 int boundary_id1,Point grid_point, 
                 Point boundary_point2, int boundary_id2,
                 Point shock_point,double *surface, 
                 Region_Stats *region,int height,int width);


/**************************************************************************
********************************force.c************************************
**************************************************************************/

double compute_edge_force(double first_der);

double compute_rotation_force_line(Point pt);

double compute_interp_force(Point Pt, Point gridPt, 
                ENO_Interval_Image_t *enoimage);

double compute_statistics_force(double interp, Region_Stats *region, int multi_gauss);

void compute_forces(SP_Bubble_Deform *spbub_deform);

void expand_to_all_level_sets(Heap *heap,Cedt *cedt,double *zero_level_set,
                  double *array, SP_Bubbles_Params params);

double distance_drop_off_function(double distance);

double compute_coupled_statistics_force(Point boundary_point1, Point shock_point, Point boundary_point2,
                int region_label1,int region_label2, Region_Stats *region,int no_of_classes,
                ENO_Interval_Image_t *enoimage,int draw);
double compute_coupled_line_force(Point boundary_point1, Point shock_point, 
                  Point boundary_point2, int height, int width, 
                  int region_label1,int region_label2, 
                  double *line_force,int draw);
double compute_coupled_curvature(Boundary *boundary, Point boundary_point1, int boundary_id1,
                   Point grid_point, Point boundary_point2, int boundary_id2,
                   Point shock_point,double *surface, Region_Stats *region,int height,int width);


double compute_coupled_edge_force(Point boundary_point1, Point shock_point, 
                 Point boundary_point2,
                 ENO_Interval_Image_t *enoimage);

void compute_force_at_point(Point grid_point, SP_Bubble_Deform *spbub_deform);


void force_draw_ps(Point boundary_point, Point shock_point,double force, int color);

/**************************************************************************
**************************deform_SP_bubbles.c******************************
**************************************************************************/

void deform_SP_bubbles(SP_Bubble_Deform *spbub_deform);

void subpixel_bubble_evolution(SP_Bubble_Deform *spbub_deform);

void get_zero_level_set_cedt(SPCedt *cedt, double *zero_level_set, int height, int width);


/**************************************************************************
**********************************flux.c***********************************
**************************************************************************/
void reaction_flux(double *surface_array,double *reaction_array,int band_size, 
           double flow_direction, int x, int y, int height, int width);

void reaction_flux_with_eno(double *surface_array,double *reaction_array,
                double band_size, double flow_direction, 
                ENO_Interval_Image_t *enoimage, 
                int x, int y, int height, int width);

void diffusion_flux_with_eno(double *surface_array,double *diffusion_array,
                 double band_size,ENO_Interval_Image_t *enosurface, 
                 int x, int y, int height, int width);

void diffusion_flux(double *surface_array, double *diffusion_array,
            int band_size, int x, int y, int height, int width);


double hj_flux_osher_sethian (double u_l, double u_r, double u_u, double u_d);

/**************************************************************************
*****************************save_results.c********************************
**************************************************************************/
void save_2d_ED_bubbles_results(char *outimage, double *surface_array, double *level_set_array, 
                             double alpha, double beta, int how_often_save,int save_surface,
                             int iteration, double band_size, int height, int width);

/**************************************************************************
******************************snake_force.c********************************
**************************************************************************/

double find_grid_snake_force(double *Sx,double *Sy,ENO_Interval_Image_t *enosurface, int x, int y,
                int height, int width);

double find_grid_snake_force_old(Point boundary_point,ENO_Interval_Image_t *enoimage,
                ENO_Interval_Image_t *enosurface);

void compute_subpixel_snake_force(Point grid_point, Point boundary_point,
                  ENO_Interval_Image_t *enoimage, 
                  ENO_Interval_Image_t *enosurface, double *snake_force);


double find_grid_snake_force_cent_diff(Point boundary_point,double *x_grad,double *y_grad,
                      ENO_Interval_Image_t *enosurface);

double find_grid_stop_grad_cent_diff(double *x_grad,double *y_grad,int x,int y, 
                    int height,int width, double *Sx, double *Sy);

double upwind_derivatives(double u_l, double u_r);

double find_grid_snake_force_hessian(double *Ixx_a, double *Iyy_a, double *Ixy_a, 
                    double *Iyx_a, double *Iy_a, double *Ix_a, 
                    ENO_Interval_Image_t *enosurface,
                    Point boundary_point);


/**************************************************************************
****************************inter-bubble-shocks.c**************************
**************************************************************************/
int convert_boundary_to_linear(Boundary *boundary,Boundary *linear_boundary);

int convert_selected_boundary_to_linear(Boundary *boundary,
                     Boundary *linear_boundary,
                     int *segment_list);

void draw_shock_and_boundary(Shocks *shocks, Boundary *boundary, int draw_line);
void draw_shock_and_boundary_crop(Shocks *shocks, Boundary *boundary, int draw_line,
                  int xstart, int ystart, int offset);

void convert_higher_shocks(Shocks *shocks, Boundary *boundary);

int update_shock_labels_on_boundary(Shocks *shocks, SPCedt *cedt);

void print_shock_labels_all(Boundary *boundary);

void print_shock_labels(Boundary *boundary);

void geno_approx_of_shocks(Shocks *shocks, Boundary *boundary, int *back_shock_ptr, 
               int height, int width);

int find_correct_shock_boundary_id(Boundary *shock_boundary, Boundary *boundary,
                   int *back_shock_ptr,int lsh_label, int rsh_label,
                   int lb_label,int rb_label);

void find_line_geno_boundary_intersection(Boundary *boundary, int boundary_id,
                      Point grid_point, Point boundary_point,
                      Point *Sol);

int find_shock_point_for_boundary(Boundary *shock_boundary, Boundary *boundary,
                  Shocks *shocks,int *back_shock_ptr, Point grid_point,
                  Point *boundary_point, int boundary_id, 
                  Point *shock_point,int height,int width);

int find_boundary_point_for_shock(Boundary *shock_boundary, SPCedt *cedt, Shocks *shocks,
                  Point shock_point, int shock_boundary_id,
                  Point first_boundary_point,int first_boundary_id, 
                  Point *second_boundary_point);
int get_multiple_shock_labels(Boundary *boundary,int boundary_id,Point boundary_point,
                  int **shock_labels);

int get_multiple_shock_labels_intv(Boundary *boundary,int boundary_id,Point boundary_point,
                   int **shock_labels);


Point get_shock_gen_point(Boundary *boundary, int boundary_id, int shock_label);

void initialize_inter_bubble_shocks(Shocks *shocks,int height,int width);

void free_shock_grids_ptrs(Shocks *shocks, int height, int width);

void free_memory_for_hosh(Shocks *shocks, int size);


void find_inter_bubble_shocks(SP_Bubble_Deform *spbub_deform);


/**************************************************************************
 ************************************utils.c*******************************
**************************************************************************/
int is_valid_label(int label);

int is_valid_point(Point pt);

void make_signed_distance_surface(double *surface,unsigned char *seed,
                  int height,int width, int sqr_flag);
/**************************************************************************
*****************************shock_to_cedt_boundary.c**********************
**************************************************************************/
void cedt_boundary_with_geno_labelled(Tracer *Tr, Boundary *boundary,int height,
                      int width);
void initialize_cedt_boundary_point_labelled(double *contour_array,
                         Boundary *boundary,
                         int *boundary_index,
                         int contour_index,
                         int *contour_id_array);

void initialize_cedt_boundary_23points_labelled(double *contour_array,
                         Boundary *boundary,
                         int *boundary_index,
                         int contour_index,
                         int *contour_id_array,         
                         int no_of_points);

void 
initialize_cedt_boundary_geno_curve_labelled(double *contour_array,
                         Boundary *boundary,
                         GENO_Interpolated_Curve *icurves,
                         int *boundary_index,
                         int contour_index,
                         int *contour_id_array,
                         int no_of_points,
                         int closed);

/**************************************************************************
*********************************level_set.c****************************
**************************************************************************/
void get_level_set_surface(double *surface_array, double *level_set_array, 
               double level, int boundary_value, int interpolate, 
               int band_size, int height, int width);

void find_zero_crossing_double(double *level_set_array, double *surface_array, 
                   double point_value, 
                   double level, int boundary_value, int y, int x, 
                   int interpolate, int height, int width);

void linear_interpolation_double(double *level_set_array, double interpolation, 
                 double boundary_value,
                 int position, int new_position);

/**************************************************************************
*********************************merge_regions.c****************************
**************************************************************************/
void merge_all_similar_regions(SP_Bubble_Deform *spbub_deform);

void mark_band_around_common_boundary(Boundary *boundary, int *region_label_array,
                      int *marked_label_array, int region_label1,
                      int region_label2, int height, int width);

int should_regions_be_merged(double *image, int *marked_label_array, 
                 int region_label1,int region_label2, int bndry_length,
                 int height, int width);

void update_new_region_label_array(int *region_label_array, 
                   int *new_region_label_array,
                   int i, int j, int merge,
                   int height, int width);
/**************************************************************************
*********************************initialize_raphael.c***********************
**************************************************************************/

void initialize_sp_bub_deform_raph(int *in_region, unsigned char* in_seed,
                   double *in_surface, double *in_image,
                   SP_Bubble_Deform * spbub_deform);

void run_scdm_raph(double alpha, double beta, double gamma,
           int no_iter, int start_from, int pad,
           int height, int width, int save_levelset,
           int save_surface, double gauss_smooth, 
           int curv_smooth, int region_flag, char *outbase,
           int *in_region, unsigned char * in_seed,
           double *in_surface, double *in_image);

void read_input_bubble_arrays_raphael(double *image, unsigned char *seed, 
                      double *surface, int *region, 
                      double *in_image, unsigned char *in_seed, 
                      double *in_surface, int *in_region, 
                      SP_Bubbles_Params *params);

/**************************************************************************
*********************************.c****************************
**************************************************************************/


#endif

#ifdef __cplusplus
}
#endif

