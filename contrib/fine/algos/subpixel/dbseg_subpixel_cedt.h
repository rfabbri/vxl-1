#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CEDT_H_
#define _CEDT_H_

#define LARGE 99999.0;

typedef struct {
  float *dx, *dy;
  float *surface;
  int *dir, *region;
}Cedt;

typedef struct {
  int y,x;
  int py,px; /* previous location */
  int nx,ny; /* next location */
}Location;


void dist_trans_by_cedt(float *surface, float band_size, int height, int width); 
int initial_direction(int x, int y);
void speed_expansion_by_cedt(Heap *heap, Cedt *cedt, float *level, float *speed, 
                             float *speed_exp, float band_size, int height, int width); 
void propagate_dist(Heap *heap, Cedt *cedt, Location *Loc, int dir, int height, int width);
void initial_diagonal_propagate_dist(Heap *heap, Cedt *cedt,Location *Loc, int dir, 
                                     int height, int width);
void propagate_speed(Heap *heap, Cedt *cedt, Location *Loc, float *speed_exp,
                     int dir, int height, int width);
void initial_diagonal_propagate_speed(Heap *heap, Cedt *cedt,Location *Loc, 
                                      float *speed_exp, int dir,int height,int width);
void add_to_contour_and_heap(Heap *heap, Cedt *cedt, float dist_x, float dist_y, 
                             int y, int x, int dir, int height, int width);


#endif /* CEDT */

#ifdef __cplusplus
}
#endif

