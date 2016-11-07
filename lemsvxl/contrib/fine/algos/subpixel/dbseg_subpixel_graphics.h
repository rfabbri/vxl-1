#ifdef __cplusplus
extern "C" {
#endif

#ifndef _GRAPHICS_TBS_H_
#define _GRAPHICS_TBS_H_

#include <stdio.h>
#include <stdlib.h>
#include "dbseg_subpixel_1d_eno_types.h"


/********************
  ::::::::::::::
  contour_tracer.h
  ::::::::::::::
  *******************/

#define NOXING -11
#define TRACED -12
#define BOUNDARY -13
#define ENDOFCONTOUR -14
#define ENDOFLIST -15
#define ENDOFXINGS -16
#define CLOSEDCONTOUR -21
#define OPENCONTOUR -22


#define TR_VERY_LARGE_VALUE 1E10
#define TR_VERY_SMALL_VALUE 1E-3

#define TR_GRID -1
#define TR_HORIZ -2
#define TR_VERT -3
#define TR_CELL -4


typedef struct {
   double x,y;
   int type;
}CurrPt;

typedef struct {
  int size;
  int type;
  int *length;
  double *vert, *horiz;
  int *label;
  int *id;
}Tracer;

typedef struct{
  int cnt;
  double dist;
  double loc[2];
  int label[2];
}XingsLoc;

typedef struct {
  int size;
  int type;
  XingsLoc  *vert, *horiz;
}Xings;


typedef struct {
  double y,x;
}Tr_Point;

void subpixel_contour_tracer_all(Tracer *Tr, Xings *xings, int height,
                 int width); 
void subpixel_contour_tracer(Tracer *Tr, CurrPt *start_pt, Xings *xings, 
                 double label, int height, int width) ;



/********************
  ::::::::::::::
  contour_processing.h
  ::::::::::::::
  ********************/
void merge_same_contours(Tracer *tracer, Tracer *tracer_final);

void label_traced_contours(Tracer *Tr, ENO_Interval_Image_t *enoimage);

void remove_xings_due_to_padding(Tracer *Tr, int height, int width, int pad);

void fix_tracer_labels(Tracer *Tr);

void place_zerox_by_label(ENO_Interval_Image_t *enoimage, double *surface,
              int *region_label_array, int height, int width);
/********************
  ::::::::::::::
  eno_zero_tracer.h
  ::::::::::::::
  *******************/
void trace_eno_zero_xings(double *surface, Tracer *Tr, int height, int width); 

void  trace_eno_zero_xings_labelled(double *surface, Tracer *Tr, int *label,
                    int height, int width);

void contour_tracer_using_signed_dt(int label, int *image_array, 
                    double *contour_list, double smoothing,
                    int height, int width);
void  trace_eno_zero_xings_create_labels(double *surface, Tracer *Tr,
                     int height, int width);
void trace_eno_zero_xings_raph(double *surface, double *contour_list,
                   int height, int width);
void trace_eno_zero_xings_labelled_raph(double *surface, double *contour_list,
                    int *label_array, int height, int width);

/********************
  ::::::::::::::
  next_xing_location.h
  ::::::::::::::
  *****************/

int next_xing_location(CurrPt *curr_pt,CurrPt *next_pt,CurrPt *end_pt,
               XingsLoc *vert_xings, XingsLoc *horiz_xings, int sss,
               double label, int height, int width);

int one_d_crossing(XingsLoc *xings, CurrPt *next_pt, CurrPt *curr_pt,
           CurrPt *end_pt,double label,int x,int y,int order, 
           int direction, int height, int width);

int corner_crossing(XingsLoc *xings, CurrPt *next_pt,CurrPt *curr_pt,
            CurrPt *end_pt, double label,int x,int y, int corner_x, 
            int corner_y, int order, int direction, int height, 
            int width);

/********************
  ::::::::::::::
       utils.h
  ::::::::::::::
  *****************/


void tr_compute_directions(Tr_Point a, Tr_Point b, int *xdir, int *ydir);
double tr_find_slope(Tr_Point a, Tr_Point b);
int tr_xing_type(Tr_Point *Pt); 
void tr_find_line_equation(Tr_Point a, Tr_Point b, double *slope, double *constant);
int tr_almost_equal(double a, double b);
void free_data_tracer(Tracer *Tr);

/********************
  ::::::::::::::
  direction.h
  ::::::::::::::
  ******************/

int find_initial_tracer_direction(Tr_Point curr, Tr_Point prev, int *sign);

/***************************/


#endif

#ifdef __cplusplus
}
#endif

