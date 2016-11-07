#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WAVE_PROPAGATION_H_
#define _WAVE_PROPAGATION_H_

#include <utils.h>
#include <graphics.h>

#define ERROR1 -25

#define REGULAR_WAVE -1
#define RAREFACTION_WAVE -2
#define SHOCKWAVE -21
#define RAY1 -22
#define RAY2 -23


#define REGULAR 0
#define DEGENERATE 1
#define SEMI_DEGENERATE 2

#define POINT -1
#define LINE -2
#define ARC -3
#define JOINT -4
#define MJOINT -5

#define GRID -1
#define HORIZ -2
#define VERT -3
#define CELL -4

#define D_SD -22
#define SD_R -23
#define R_SD -24
#define SD_DD -25
#define DD_SD -26
#define D_D -27
#define NO_T -28


#define SEGMENT_END -21

#define ENDOFBOUNDARY -12
#define BACKGROUND -13

#define ENDOFSHGROUP -9
#define ENDOFSHOCKS -10

#define INWARD -19
#define OUTWARD -20

#define JUNCTION -92
#define SOSH -93
#define FOSH -94
#define TOSH -95


#define LEFT_TURN -33
#define RIGHT_TURN -34

#define LEFT_SW -42
#define RIGHT_SW -43

#define STOPPED 2
#define DELETED -1
#define PRUNED -2
#define IGNORED -3
#define MERGED -4

#define P_INF -99.0
#define M_INF -99.0
#define INF -99.0
#define START_PT 0.0
#define END_PT 1.0

/********************************** Boundary *********************************/
typedef struct {
  int boundary, boundary_init, boundary_tracer;
  int sw_propagation, sw_draw, sw_utils;
  int SO_shock, SO_shock_detection, sw_from_SO;
  int sw_from_boundary, sw_from_junction, junctions, junction_detection;
  int transition_detection, next_transition;
  int dwaves_init, update_dwaves, dwaves_draw;
  int main_update,level;
}Debug;

typedef struct {
  int smoothing,grow;
  double zoom, xstart, ystart;
  double band_size;
  int cr_start, cr_end;
  int inside;
  int debug;
}Pars;

typedef struct {
  double y,x;
}Point;

typedef struct {
  double y1,x1;
  double y2,x2;
  double normal;
  double tangent1, tangent2;
}Line;

typedef struct {
  double y1,x1;
  double y2,x2;
  double center_y,center_x;
  double radius;
}EliArc;

typedef struct {
  double y,x;
  int neighbor_no;
  int *n_id;
  int *joint_id;
}Joint;

typedef struct {
  int type;
  double y1,x1,y2,x2;
  Point center;
  double radius;
  double normal[2];
}GeometricModel;


typedef struct {
  int start_id, end_id;
  int size;
}BSegmentId;

typedef struct Link_S Link;

struct Link_S {
  int id;
  double loc;
  Link *next;
};

typedef struct {
  int cnt;
  Link *link;
  Link *init;
}LinkList;

typedef struct Quench_S Quench;

struct Quench_S {
  int label;
  double loc;
  Quench *next;
};

typedef struct {
  Quench *quench;
  Quench *init;
}QWaves;


typedef struct {
  GeometricModel model;
  Joint joint;
  QWaves SO_qid_list;
  LinkList SO_shock_list;
  int type,id,segment;
  int n_id[2]; /* neighboring boundary segments */
  int joint_id[2];
  int inward_sw[2], outward_sw[2];
  int del, group_id;
}BoundaryId;



typedef struct {
  BoundaryId *id;
  BSegmentId *segment;
  int start, end, size,bgroup_ptr;
  int length, joint_offset,joint_end,segment_ptr;
  int height, width;
  Debug *debug;
}Boundary;


/********************** DiscreteWaves *************************************/

typedef struct {
  double *dx, *dy;
  double *surface;
  int *label;
  unsigned char *tag, *type, *dir, *mask;
  QWaves *qwaves;
  int height, width;
  Debug *debug;
}DiscreteWaves;


/********************************ShockWave *****************************/

typedef struct {
  int wtype[2];
  Point endPt;
  double enddist;
  double boundary_t[2];
}Path;


typedef struct {
  int ttype,ftype,init,del,transition,junction;
  int sh_id, init_sh_id,curr_init_sh_id,end_sh_id,jsh_id;
  int ignored, pruned;
  Point origin,currPt,nextPt, prevPt, endPt;
  double m, b0;
  double direction, y_dir,x_dir,saliency;
  int boundary_id[2];
  int boundary_loc[2], next_boundary_loc[2];
  double boundary_t[2];
  double origin_t[2];
  int wtype[2],twtype[2];
  double dist,nextdist,enddist;
  int r_sw, l_sw,q_sw,ql_sw,qr_sw,p_no;
  int shock_label, shock_order;
  int p_sw[2], c_sw[2];
  int merged,merge_id, border;
  int wdir[2];
  int path_ptr;
  Path path[9];
  int origin_sw[2];
  int end,type;
  int level_id;
}ShockWaveId;

typedef struct {
  ShockWaveId *id;
  int sw_ptr,sw_offset;
  int start, end, size;
  int joint_offset;
  int final_sw[2];
  int height, width;
  Debug *debug;
}ShockWaves;

/******************************* Shocks *****************************/

typedef struct {
  int group_id;
  int hosh_id;
  int sw_id, next_id, prev_id;
  double y,x,dist,saliency;
  double speed[3], direction[3];
  unsigned char label, order, del, pruned;
  int boundary_id[6];
  double t[2];
  Point boundary[6];
}ShockId;

typedef struct {
  ShockId *id;
  int ptr, offset, size;
  int height, width;
  Debug *debug;
}Shocks;

typedef struct {
  Boundary boundary;
  DiscreteWaves dwaves;
  ShockWaves shockwaves;
  Shocks shocks;
  Heap heap;
  Debug *debug;
}WavePropagation;

#endif /* SUBPIXEL_SHOCKS */

#ifdef __cplusplus
}
#endif

