#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SUBPIXEL_SHOCKS_H_
#define _SUBPIXEL_SHOCKS_H_

#include "dbseg_subpixel_utils.h"
#include "dbseg_subpixel_graphics.h"

#define REGULAR_WAVE -1
#define RAREFACTION_WAVE -2

#define REGULAR 0
#define DEGENERATE 1
#define SEMI_DEGENERATE 2

#define POINT -1
#define LINE -2
#define ARC -3

#define GRID -1
#define HORIZ -2
#define VERT -3
#define CELL -4

#define ENDOFBOUNDARY -12
#define BACKGROUND -13

#define MIN_NEIGHBORS 3
#define MIN_ANGLE 10

#define ENDOFSHGROUP -9
#define ENDOFSHOCKS -10

#define INWARD -19
#define OUTWARD -20

typedef struct {
  double y,x;
}Point;

typedef struct {
  Point currPt;
  Point origin;
  double m, b0;
  int label;
  int type,front_id;
  int direction;
  double ydir,xdir;
  double dy,dx;
}Ray;

typedef struct {
  double y1,x1;
  double y2,x2;
  double tangent1,tangent2;  /*Store tangents at start and end points */
  int xndir,yndir; /*Store discrete normal direction*/
  int xtdir,ytdir; /*Store discrete tangent direction*/
  double nslope1,nconst1;  /*Store equation of normal line direction (x1,y1)*/
  double nslope2,nconst2;  /*Store equation of normal line direction (x2,y2)*/
  double normal;
}Line;

typedef struct {
  double y1,x1;
  double y2,x2;
  double tangent1,tangent2;  /*Store tangents at start and end points */
  int x1ndir,y1ndir;  /*Store discrete normal direction for (x1,y1)*/
  int x2ndir,y2ndir;  /*Store discrete normal direction for (x2,y2)*/
  int x1tdir,y1tdir;  /*Store discrete tangent direction for (x1,y1)*/
  int x2tdir,y2tdir;  /*Store discrete tangent direction for (x2,y2)*/
  double nslope1,nconst1;  /*Store equation of normal line direction (x1,y1)*/
  double nslope2,nconst2;  /*Store equation of normal line direction (x2,y2)*/
  double center_y,center_x;
  double radius,curvature;
}EliArc; //eli edit

typedef struct ShockLabel_s{
  Point loc;
  int label;
  double dist;
  struct ShockLabel_s *next;
}ShockLabel;

typedef struct {
  Point point;
  Line line;
  EliArc arc; // eli edit
  double normal;
  int type,id,segment;
  int left,right;
  ShockLabel *shock;
  int num_shock_gen_points;
  int start_shock_label,end_shock_label;
}Boundary;

typedef struct GridLabel_s{
  int label;
  double x,y;
  double dist;       /*The distance from the boundary segment */
  int end_type;     /*Start (1) or End (0) of interval*/
  int intv_type;    /*Top (1) or Bottom (0) - Vertical intv
              Left (1) of Right (0) - Horizontal intv*/
  struct GridLabel_s *next;
}GridLabel;

typedef struct {
  double *dx, *dy;
  double *surface;
  int *label;
  unsigned char *tag, *type, *dir;
  GridLabel *horiz, *vert;
  int *no_hor_labels,*no_ver_labels;
  Boundary *boundary;
  Ray *ray;
  int ray_ptr;
  int boundary_end,  boundary_start,  boundary_size;
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


typedef struct{
  int *id;
  int incoming_gr[10], outgoing_gr[10];
  char type;
  int size,del;
  int begin,end;
}ShGroups;

typedef struct {
  int group_id;
  int hosh_id;
  double y,x,dist;
  double speed, direction;
  unsigned char label, order, del, pruned;
  int boundary_id[2];
  Point boundary[2];
}ShId;

typedef struct {
  int id[40];
  int cnt;
}ShSpace;

typedef struct {
  ShSpace *vert;
  ShSpace *horiz;
}ShGrid;

typedef struct {
  int shock_id;
  int boundary_no;
  int no_of_att;
  int pruned;
  double *speed, *direction;
  int *boundary_id;
  Point *boundary;
  int incoming_id[10];
  int outgoing_id[10];
}Hosh;

typedef struct{
  int inter_boundary;
  int ptr;
  int group_id;
  int group_cnt;
  int hosh_ptr;
  ShId *id;
  ShGrid *grid;
  Hosh *hosh;
  ShGroups *groups;
  int *grouplist;
}Shocks;

/*void inter_boundary_shocks(Boundary *boundary, Shocks *Sh, int detect_hosh, int height, int width);*/
void inter_boundary_shocks(SPCedt *cedt, CEDTWaves *waves, Shocks *Sh, int detect_hosh, int height, int width);
double angle_between_vectors(double angle1, double angle2);

void wave_propagation_via_cedt(Heap *heap, SPCedt *cedt, CEDTWaves *waves, 
                   int height, int width);
#endif /* SUBPIXEL_SHOCKS */

#ifdef __cplusplus
}
#endif

