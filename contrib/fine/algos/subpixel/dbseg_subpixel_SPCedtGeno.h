#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SUBPIXEL_CEDT_GENO_H_
#define _SUBPIXEL_CEDT_GENO_H_

#include "dbseg_subpixel_utils.h"
#include "dbseg_subpixel_tbs-utils.h"
#include "dbseg_subpixel_graphics.h"
#include "dbseg_subpixel_geno-types.h"
#include "dbseg_subpixel_geno.h" 
#include "dbseg_subpixel_cedt.h" 
#include "dbseg_subpixel_shocks_from_cedt.h"
#include <assert.h>
#include "dbseg_subpixel_boundary_from_geno.h"
/**************************************************************************/
/******************************subpixel_cedt.h****************************/
/**************************************************************************/



#define ENDPOINT -10
#define GOOD_ENDPOINT -11
#define INVALID_POINT -10

#define START 1
#define END 0


#define TOP 1
#define BOTTOM 0
#define LEFT 1
#define RIGHT 0

/* Came from line-utils.c*/
#define VERY_LARGE_VALUE 1E10


/* Came from line-utils.c*/



/*****************************************************************************/
/********************************line-utils.c*********************************/
/*****************************************************************************/



double find_slope(Point a, Point b);

void compute_directions(Point a, Point b, int *xdir, int *ydir);

void find_line_equation(Point a, Point b, double *slope, double *constant);

void find_normal_line_equations(Point a, Point b,  double *slope, double *c1,double *c2);

void find_normal_line_directions(Point a, Point b,int *xdir, int *ydir);

void find_tangent_line_directions(Point a, Point b,int *xdir, int *ydir);

double get_correct_normal_angle(double nslope, int xndir, int yndir);

double unit_abs(double a);


int xing_type(Point *Pt); 


/**************************************************************************/
/******************************subpixel_cedt.c****************************/
/**************************************************************************/

void subpixel_cedt(SPCedt *cedt, Tracer *Tr, unsigned char *image, 
           int height, int width);

void subpixel_cedt_pad(SPCedt *cedt, Tracer *Tr, unsigned char *image, 
               int height, int width);

void subpixel_cedt_pad_pad(SPCedt *cedt, Tracer *Tr, unsigned char *image, 
               int height, int width);

SPCedt* subpixel_cedt_from_image_pad(unsigned char *image, int height, int width, 
                     int pad);
void subpixel_cedt_from_image(unsigned char *image, SPCedt *cedt, Heap *heap,
                  int height, int width);

void subpixel_cedt_from_image_labelled(unsigned char *image, SPCedt *cedt, Heap *heap,int curv_smooth,
                       int *label,int height, int width);

/**************************************************************************/
/***************************boundary_from_geno.c***************************/
/**************************************************************************/


void fix_boundary_zerox(Boundary *boundary, int height, int width);


GENO_Curve_List* makeCurvesFromArray(double* contour_array);




void geno_draw_interpolated_curves(GENO_Interpolated_Curve* curve);




void print_cedt_boundary(Boundary *boundary,int start_index,
                    int boundary_index);

void draw_cedt_boundary(Boundary *boundary,int start_index,
                        int boundary_index,int height, int width);

void find_line_end_points(Point p1, Point p2, Point *sol, int h, int w);

void cedt_boundary_ends(Boundary *boundary,int height,int width);

void find_all_normal_line_eqn_and_dir(SPCedt *cedt);




/**************************************************************************/
/********************************fileio.c**********************************/
/**************************************************************************/


void read_raw_tracer_data_file(char *file_name, Tracer *Tr, int height, int width);

double * getContourFromFile(char *contour_file_name, int height, int width);

void getTracerFromFile(char *contour_file_name, int height, int width,  Tracer *Tr);

void getTracerFromFile2(char *contour_file_name, int height, int width,  Tracer *Tr);

void writeTracerToFile(char *contour_file_name, int height, int width,  Tracer *Tr);

void writeTracerToFile2(char *contour_file_name, int height, int width,  Tracer *Tr);

/**************************************************************************/
/******************************initial_cedt.c****************************/
/**************************************************************************/


void initial_subpixel_dt(unsigned char *image,SPCedt *cedt,Heap *heap,int height, int width);

void dt_for_SPpoint_and_its_neighbors(unsigned char *image, Point *Pt1, Point *Pt2, Line *Ln,
                      SPCedt *cedt,Heap *heap,int Pt1_type,int label,
                      int height, int width);
void init_distances_and_orientations(SPCedt *cedt, Heap *heap, Point *Pt1,Line *Ln,
                     double dy, double dx,int label,int height, int width);
void init_dt_for_point(SPCedt *cedt, Heap *heap,Point *Pt1,int label,int height, int width);


/**************************************************************************/
/**************************initial_cedt_with_geno.c************************/
/**************************************************************************/


void initial_subpixel_dt_with_geno(unsigned char *image, SPCedt * cedt, 
                   Heap *heap, int height, int width);


void dt_for_SPpoint_and_neighbors(unsigned char *image,Point *Pt1,
                   SPCedt *cedt,Heap *heap,int Pt1_type, 
                   int label,int height, int width);



void init_distances_and_orientations_geno(SPCedt *cedt, Heap *heap, 
                      Point *Pt1,
                      double dy,double dx, int label, 
                      int height, int width);
void CopyLineToLine(Line *ls, Line *ld);

void CopyArcToArc(EliArc *as, EliArc *ad);

int find_arc_wave_direction(EliArc *Ac, Point *Pt);

int find_point_wave_direction(Point *a, Point *b);

void propagate_initial_diagonal_directions_arc(SPCedt *cedt, Heap *heap, int x, int y, 
                           int dir, int label, int height, int width);


/**************************************************************************/
/*****************************intersections.c******************************/
/**************************************************************************/

int intersection_of_a_point_and_line_tbs(Line *line, Point *point, Point *Sol); 

double distance_from_point_to_line(Line *line, Point *point);

int intersection_of_a_point_and_arc(EliArc *arc, Point *point, Point *Sol);

int line_segments_intersection(Point a, Point b, Point c, 
                   Point d, Point *p);
int two_line_intersection(Point a, Point b, Point c, Point d, Point *p);

double euclidean_distance(Point a, Point b);

int find_distance(SPCedt *cedt, double x, double y, int label, Point *Sol);

int find_distance_boundary(Boundary *boundary, double x, double y, Point *Sol);

int find_distance_linear_approx(SPCedt *cedt, double x, double y,
                 int label, Point *Sol);
int find_distance_cell_point(SPCedt *cedt, double x, double y, int label, Point *Sol);

int  find_intersection_circle_line(Point pt, double slope, double ct,
                    double cx, double cy, double R,
                    Point *Sol1, Point *Sol2);

int  find_intersection_arc_line(Point pt, double slope, double ct,
                double cx, double cy, double R,
                Point St, Point En,
                Point *Sol1, Point *Sol2);


/**************************************************************************/
/**********************************label.c*********************************/
/**************************************************************************/


void init_grid_labels(SPCedt *cedt,int height, int width);

void update_grid_labels(SPCedt *cedt, int xx, int yy, int label,
            int height, int width);


void insert_label_to_list(GridLabel *grid, int xx, int yy, int label,
              int end_type,int intv_type, Point p, double dist,
              int height, int width, int *label_ct);


double find_slope(Point a, Point b);


void find_grid_intersections(SPCedt *cedt, Point a, Point b, int xx, int yy, 
                 int label,int height, int width);


void find_next_grid_intersection(Point a, int xdir, int ydir, double Bdry_slope,
                 double Bdry_const, Point *loc);

void compute_directions(Point a, Point b, int *xdir, int *ydir);


void update_label_list(SPCedt *cedt, int label, int end_type, 
               Point pt, double dist,int height, int width);

void find_line_equation(Point a, Point b, double *slope, double *constant);

void find_normal_line_equation(Point a, Point b,  double *slope, double *c1,double *c2);

void find_normal_line_directions(Point a, Point b,int *xdir, int *ydir);

void find_tangent_line_directions(Point a, Point b,int *xdir, int *ydir);

double unit_abs(double a);

int find_regular_wave_at_grid(SPCedt *cedt, int x, int y, 
                  int height, int width);


/**************************************************************************/
/*****************************label_utils.c********************************/
/**************************************************************************/

int find_intv_type_horiz(SPCedt *cedt, int label, int end_type);

int find_intv_type_vert(SPCedt *cedt, int label, int end_type);


/**************************************************************************/
/***************************move_boundaries.c******************************/
/**************************************************************************/


void compute_new_boundary_points(SPCedt *cedt, Tracer *Tr, int direction,
                 int height,int width);

void change_surface_to_signed(Tracer *Tr, double *surface, int height, int width);

int find_neighboring_grid_crossings(Point *pt, int height, int width, Point *gc);



int find_if_new_point_is_good(Point *pt, SPCedt *cedt, int height, 
                  int width,int oldlabel);



double find_force(Point pt);

void change_surface_to_signed_line(double *surface, int height, int width, 
                   double theta);

void find_one_new_point(SPCedt *cedt,int label, int end_flag, Point *newPt,
            int direction);


void remove_overlapping_points(Tracer *Tr, Tracer *Tr_tmp, int count);

void subpixel_evolution_boundary(double *in_surface, unsigned char *in_image,
                double *out_surface, int height, int width, 
                int iterations,int direction);

double find_circle_rotate_force(Point pt, Point grid_pt, double slope, double ct,int i);


/**************************************************************************/
/*******************************print_cedt.c********************************/
/**************************************************************************/

#define I 360

void print_grid_labels(SPCedt *cedt, int height, int width);

void print_cedt_arrays(SPCedt *cedt, int height, int width);

void arrow_draw_ps(double x1, double y1, double x2, double y2, int color, void *data);

void vector_draw_ps(double x1, double y1, double r, double theta, int color, void *data)
;

void draw_cedt_direction(double x, double y, unsigned char dir, void *data);

void print_cedt_directions(char *filename,SPCedt *cedt, int height, int width);

void  print_cedt_info_at_point(SPCedt *cedt,int x,int y,int height,int width); 

void draw_one_cedt_boundary_segment(SPCedt *cedt, int label,int color);

void draw_cedt_boundary_segment_zoom(Boundary *boundary,int sx,int sy,int color);

void draw_one_boundary_segment_zoom(Boundary *boundary,int label,int sx,int sy,int color);

void draw_tracer(Tracer *Tr,int color);

void draw_one_cedt_boundary_segment_zoom(SPCedt *cedt, int label,int sx,int sy,int color
);

void draw_tracer_zoom(Tracer *Tr,int sx, int sy,int color);

void draw_cedt_boundary_tangents_zoom(SPCedt *cedt, int label,int sx,int sy,int color);

void draw_boundary_tangents_zoom(Boundary *boundary, int label,int sx,int sy,int color);

/*void draw_directional_vectors(double y0, double x0, int dir, double size, 
  int color);*/

void draw_cedt_direction_lookup(double x, double y, int dir, void *data);




/**************************************************************************/
/*******************************propagate.c********************************/
/**************************************************************************/

#define DC (0)

void propagate_dist_sp(Heap *heap, SPCedt *cedt, int y, int x, int dir, int height, int width);
void initial_diagonal_propagate_dist_sp(Heap *heap,SPCedt *cedt, int y, int x,
                                     int dir, int height, int width);
void add_to_contour_and_heap_sp(Heap *heap, SPCedt *cedt, double dist_x, double dist_y, 
                             int y, int x, int dir, int wave_type, 
                             int label, int height, int width);
void consider_neighboring_geno_intervals(Heap *heap,SPCedt *cedt,Point *Pt1,int dir, double dist_y,
                                    double dist_x,int yy,int xx,int label,int height,int width);
 
int check_for_regular_wave(SPCedt *cedt, int x, int y, int height, int width);



/**************************************************************************/
/******************************update_cedt.c*******************************/
/**************************************************************************/


void update_cedt(unsigned char *image, Heap *heap, SPCedt *cedt, int height, int width); 



/**************************************************************************/
/*********************************utils.c**********************************/
/**************************************************************************/


/*void initialize_boundary_segments(Tracer *Tr, Boundary *boundary, 
  int *boundary_ptr,int height, int width);*/

void initialize_cedt(SPCedt *cedt, int height, int width);

/*void initialize_heap(Heap *heap, int height, int width); */

void allocate_data_cedt(SPCedt *cedt, int height, int width, int size);

void free_data_cedt(SPCedt *cedt, int height, int width, int size);

void  alloc_mem_heap_SP_Cedt(Heap *heap, int datasize);

void free_mem_heap_SP_Cedt(Heap *heap);

void initialize_heap_SP_Cedt(Heap *heap, int size);

void free_cedt_boundary(Boundary *boundary, int size);

void free_grid_label_list(GridLabel *gr);

void free_grid_label_list_all(SPCedt *cedt, int height, int width);

void allocate_cedt_boundary(Boundary **boundary, int size);

void free_shock_label_list(ShockLabel *sh);

void free_shock_label_list_all(Boundary *boundary, int size);

void pad_tracer(Tracer *Tr_source, Tracer *Tr_dest, int pad);

void unpad_arrays_cedt_to_cedt(SPCedt *cedt_in,SPCedt *cedt_out,
                   int height, int width, int pad);
void fix_tracer_pad(Tracer *Tr, int pad, int height, int width);



/**************************************************************************/
/******************************wave_utils.c********************************/
/**************************************************************************/


#define HORIZONTAL -1
#define VERTICAL -2
#define DIAGONAL -3
#define HORIZ_DIAGONAL -4
#define VERT_DIAGONAL  -5



#define LARGE_NUMBER 99999.0

/*#define PI 3.145926*/

int find_line_wave_direction(Line *Ln, double dist_x, double dist_y);
int find_wave_direction(Line *Ln);
double boundary_normal(Boundary *boundary, double dist_y, double dist_x, int height, int width);
void add_to_cedt_sp(SPCedt *cedt, Point *Pt1, Point *Sol, double dist_y, double dist_x, 
                 int tag, int wave_type, int label, int height, int width);
int wave_direction(int dir, int dy, int dx); 
int sign_from_direction(int dir, double *dx_sign, double *dy_sign); 
double compute_angle_between_fronts(double angle1, double angle2);
int initial_direction_sp(int x, int y);


/**************************************************************************/
/**************************boundary_shock_label.c**************************/
/**************************************************************************/
Point get_end_pt(Boundary *boundary);

Point get_start_pt(Boundary *boundary);

ShockLabel * new_ShockLabel_element(double x, double y, int label, double distance);

void insert_shock_label_list(SPCedt *cedt, Point shock_loc, int boundary_label,
                 int shock_label);

int get_first_shock_label(Boundary *cedt_boundary);

int get_last_shock_label(Boundary *cedt_boundary);

int get_next_label_on_left(Boundary *cedt_boundary, int label, Point loc,
               int *curr_label);

int get_next_label_on_right(Boundary *cedt_boundary, int label, Point loc,
               int *curr_label);

void propagate_shock_labels(Boundary *cedt_boundary);

/**************************************************************************/
/**************************************************************************/



#endif

#ifdef __cplusplus
}
#endif

