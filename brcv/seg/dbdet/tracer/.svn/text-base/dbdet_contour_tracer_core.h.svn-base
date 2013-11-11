// This is dbdet_contour_tracer_core.h
#ifndef dbdet_contour_tracer_core_h
#define dbdet_contour_tracer_core_h
#ifdef dbdet_contour_tracer_internal_code
//:
//\file
//\brief Defns of old C-based contour tracer by Tek, TBS, Chris, et. al.
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 10/28/2005 03:09:20 PM EDT
//


#include <dbnl/algo/dbnl_eno_zerox_label.h>

#include <math.h>
#include <stdio.h>

typedef struct {
  int size;
//  int type;
  int *length;
  double *vert, *horiz;
//  int *label;
//  int *id;
} Tracer;

typedef struct{
  int cnt;
  double dist;
  double loc[2];
  int label[2];
} XingsLoc;

typedef struct {
  int size;
  int type;
  XingsLoc  *vert, *horiz;
} Xings;

typedef struct {
  double y,x;
} Tr_Point;

typedef struct {
   double x,y;
   int type;
} CurrPt;


#define sqr(x) (x)*(x)

#define UNLABELLED -1
#define NOXING -11
#define BOUNDARY -13
#define ENDOFCONTOUR -14
#define ENDOFLIST -15

#define TR_GRID -1
#define TR_HORIZ -2
#define TR_VERY_LARGE_VALUE 1E10
#define TR_VERY_SMALL_VALUE 1E-3
#define TR_VERT -3
#define TR_CELL -4

int tr_xing_type(Tr_Point *Pt);
int next_xing_location(CurrPt *curr_pt,CurrPt *next_pt,CurrPt *end_pt,XingsLoc  *vert_xings, 
                        XingsLoc *horiz_xings, int sss, double label, int height, int width);

#endif // dbdet_contour_tracer_internal_code
#endif // dbdet_contour_tracer_core_h
