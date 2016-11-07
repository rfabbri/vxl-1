#ifdef __cplusplus
extern "C" {
#endif

#ifndef _TBS_UTILS_H
#define _TBS_UTILS_H

#include <stdio.h>
#include <stdlib.h>

/******************************tbs-contour-routines.h***************************************/

#define MAX_POINTS 5000
#define DIM 2

typedef struct{
  int x,y;
} iPoint;

typedef struct{
  float x,y;
} fPoint;

typedef struct{
  fPoint *vertex;
} fPolygon;

typedef struct{
  iPoint *vertex;
} iPolygon;

int InPolygon(iPoint pt, iPolygon poly, int n);
int InFPolygon(fPoint pt, fPolygon poly, int n);
void Print_vertices(iPolygon poly, int n);
int read_from_raw_contour_file(char * file_name, fPoint * contour_points);
int read_from_raw_int_contour_file(char * file_name, iPoint * contour_points);

/***************************************************************************************/

/***********************************simple-utils.h**************************************/
/*Error hanndler*/
void error_msg(char error_text[]);
int opt_compare(char *argument, char *keyword, int length);
void invertUchar_bin_array(unsigned char *src, unsigned char *dest, int size);
void threshold(unsigned char *orig, unsigned char *final, int *lbound,
           int *ubound, int no_of_classes, int size);
void threshold_ushort(unsigned short *orig, unsigned char *final, int *lbound,
           int *ubound, int no_of_classes, int size);
void OR_arrays(unsigned char *a, unsigned char *b, unsigned char *dest, 
           int val, int size);
void AND_Arrays(float *a, float *b,  float *output,int val, int height,  int width); 
float fmin(float,float);
float fmax(float,float);
double dmin(double,double);
double dmax(double,double);
int imax(int a, int b);
int imin(int a, int b);
void block_average(float *in_array, float *out_array, int height, int width, 
           int block_height, int block_width);
double bilinear_interpolation(double a00, double a10, double a01, double a11, 
                  double x, double y);
void bilinear_interpolation_array(double *in_array, double *out_array,
                  int height, int width, int zoom);
double bilinear_interpolate_in_array(double x, double y, double *a, int height, int width);
void linear_interpolation_slices(double *in_array1, double *in_array2,
                  int height, int width, int no_slices,
                  double *out_array);
/***************************************************************************************/
/********************************SeedFill.h************************************/
/*
 * A Seed Fill Algorithm
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * fill.c : simple seed fill program
 * Calls pixelread() to read pixels, pixelwrite() to write pixels.
 *
 * Paul Heckbert    13 Sept 1982, 28 Jan 1987
 */

typedef struct {        /* window: a discrete 2-D rectangle */
    int x0, y0;            /* xmin and ymin */
    int x1, y1;            /* xmax and ymax (inclusive) */
} Window;

typedef int Pixel;        /* 1-channel frame buffer assumed */

typedef struct {short y, xl, xr, dy;} Segment;
/*
 * Filled horizontal segment of scanline y for xl<=x<=xr.
 * Parent segment was on line y-dy.  dy=1 or -1
 */

#define MAX 100000        /* max depth of stack */

#define PUSH(Y, XL, XR, DY)    /* push new segment on stack */ \
    if (sp<stack+MAX && Y+(DY)>=win->y0 && Y+(DY)<=win->y1) \
    {sp->y = Y; sp->xl = XL; sp->xr = XR; sp->dy = DY; sp++;}

#define POP(Y, XL, XR, DY)    /* pop segment off stack */ \
    {sp--; Y = sp->y+(DY = sp->dy); XL = sp->xl; XR = sp->xr;}

/*
 * fill: set the pixel at (x,y) and all of its 4-connected neighbors
 * with the same pixel value to the new pixel value nv.
 * A 4-connected neighbor is a pixel above, below, left, or right of a pixel.
 */

void  fill(int x, int y, int width, Window *win, unsigned char *pixel_array,int nv);

/***************************************************************************************/

#endif

#ifdef __cplusplus
}
#endif

