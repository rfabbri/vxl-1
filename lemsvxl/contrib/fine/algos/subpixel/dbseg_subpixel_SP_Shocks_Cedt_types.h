#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SUBPIXEL_SHOCKS_CEDT_TYPES_H_
#define _SUBPIXEL_SHOCKS_CEDT_TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define REGULAR_WAVE -1
#define RAREFACTION_WAVE -2

#define REGULAR 1
#define DEGENERATE 2
#define SEMI_DEGENERATE 3

#define POINT -1
#define LINE -2
#define ARC -3

#define ENDOFBOUNDARY -12
#define BACKGROUND -13

#define GRID -1
#define HORIZ -2
#define VERT -3
#define CELL -4

#define MIN_NEIGHBORS 3
#define MIN_ANGLE 5

typedef struct {
  double y,x;
}Point;

typedef struct {
  double y1,x1;
  double y2,x2;
  float tangent1,tangent2;  /*Store tangents at start and end points */
  int xndir,yndir; /*Store discrete normal direction*/
  int xtdir,ytdir; /*Store discrete tangent direction*/
  float nslope1,nconst1;  /*Store equation of normal line direction (x1,y1)*/
  float nslope2,nconst2;  /*Store equation of normal line direction (x2,y2)*/
  double normal;
}Line;

typedef struct {
  double y1,x1;
  double y2,x2;
  float tangent1,tangent2;  /*Store tangents at start and end points */
  int x1ndir,y1ndir;  /*Store discrete normal direction for (x1,y1)*/
  int x2ndir,y2ndir;  /*Store discrete normal direction for (x2,y2)*/
  int x1tdir,y1tdir;  /*Store discrete tangent direction for (x1,y1)*/
  int x2tdir,y2tdir;  /*Store discrete tangent direction for (x2,y2)*/
  float nslope1,nconst1;  /*Store equation of normal line direction (x1,y1)*/
  float nslope2,nconst2;  /*Store equation of normal line direction (x2,y2)*/
  float center_y,center_x;
  float radius,curvature;
}EliArc;

typedef struct ShockLabel_s{
  Point loc;
  int label;
  float dist;
  struct ShockLabel_s *next;
}ShockLabel;

typedef struct {
  Point point;
  Line line;
  EliArc arc;
  float normal;
  int type,id,segment;
  int left,right;
  ShockLabel *shock;
  int num_shock_gen_points;
  int start_shock_label,end_shock_label;
}Boundary;

typedef struct GridLabel_s{
  int label;
  float x,y;
  float dist;       /*The distance from the boundary segment */
  int end_type;     /*Start (1) or End (0) of interval*/
  int intv_type;    /*Top (1) or Bottom (0) - Vertical intv
              Left (1) of Right (0) - Horizontal intv*/
  struct GridLabel_s *next;
}GridLabel;

typedef struct {
  float *dx, *dy;
  float *surface, *normal;
  int *label;
  unsigned char *tag, *type, *dir;
  GridLabel *horiz, *vert;
  int *no_hor_labels,*no_ver_labels;
  Boundary *boundary;
  int boundary_end,  boundary_start;
}SPCedt;

typedef struct {
  int y,x,pos;
}QuenchOrder;

typedef struct Quench_S Quench;

struct Quench_S {
  int label[2];
  Quench *prev;
  Quench *next;
};

typedef struct {
  Quench *quench;
}QWaves;

typedef struct {
  QWaves *qwaves;
  QuenchOrder *qorder;
  int inter_boundary;
  int qptr;
}CEDTWaves;


typedef struct {
  int group_id;
  double y,x,dist;
  double speed, direction;
  unsigned char label, order, del;
  int boundary_id[2];
  Point boundary[2];
}ShId;

typedef struct {
  int id[20];
  int cnt;
}ShSpace;

typedef struct {
  ShSpace *vert;
  ShSpace *horiz;
}ShGrid;

typedef struct {
  int *group_id;
  double *speed, *direction;
  int *boundary_id;
  Point *boundary;
}Hosh_attributes;

typedef struct{
  int inter_boundary;
  int ptr;
  int group_id;
  int hosh_ptr;
  ShId *id;
  ShGrid *grid;
  Hosh_attributes *hos_att;
  int *grouplist;
}Shocks;

#endif

#ifdef __cplusplus
}
#endif

