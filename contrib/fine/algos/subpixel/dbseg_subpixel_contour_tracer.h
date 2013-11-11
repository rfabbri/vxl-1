#ifdef __cplusplus
extern "C" {
#endif

#ifndef _contour_tracer_h_
#define _contour_tracer_h_

#define NOXING -11
#define TRACED -12
#define BOUNDARY -13
#define ENDOFCONTOUR -14
#define ENDOFLIST -15
#define ENDOFXINGS -16

typedef struct {
   float x,y;
   int type;
}CurrPt;

typedef struct {
  int size;
  int type;
  float *vert, *horiz;
}Tracer;

typedef struct{
  int cnt;
  float dist;
  float loc[2];
}XingsLoc;

typedef struct {
  int size;
  int type;
  XingsLoc  *vert, *horiz;
}Xings;

void subpixel_contour_tracer_all(Tracer *Tr, Xings *xings, int height,int width); 
void subpixel_contour_tracer(Tracer *Tr, CurrPt *start_pt, Xings *xings, float label, 
                    int height, int width) ;
void trace_eno_zero_xings(double *surface, Tracer *Tr, int height, int width); 

#endif 

#ifdef __cplusplus
}
#endif

