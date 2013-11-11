#ifdef __cplusplus
extern "C" {
#endif

#ifndef _FAST_MARCHING_H_
#define _FAST_MARCHING_H_

#define LARGE 99999.0

#include <utils.h>

typedef struct {
  int ptr;
  double *vert, *horiz;
}Zeros;

typedef struct {
  int x, y;
}IntPoint;

void reinitialize_surface_by_fm(double *surface, double *level, Heap *heap, double band_size,
                                int height, int width);
void fast_marching(double *surface, double *tag_array, int band_size, Heap *heap,
                   int height, int width);


void initialize_fm(double *surface, double *level, Heap *heap,int height, int width);
void add_to_min_heap(Heap *heap, double dist, int y, int x, int height, int width);


/* functions defined in upwind.c */
double find_roots(double a, double b, double c);
double eikonal_upwind_solution(double *surface_array, IntPoint *curr_pt, double flux, 
                               int height, int width);
int check_upwind_solution(double *surface_array, IntPoint *curr_pt, double solution, 
                          double flux, int height, int width);



#endif /* FAST_MARCHING */

#ifdef __cplusplus
}
#endif

