#ifndef __SUSAN_SUB_H
#define __SUSAN_SUB_H
#include "susan.h"
int susan_response(uchar * in,uchar * bp,int max_no,int x_size,int y_size,int i,int j,uchar intensity,double angle);
struct Susanpixels * susan_subpixel(uchar * in,uchar * bp,int *r, int max_no,uchar *mid, int x_size, int y_size,int * a,int *b,int *w,uchar * do_sym,char * angle_edges,int mask_size);
void susan_subpixel1(int *r,uchar *mid,int x_size, int y_size,int *a,int *b,int *w);
#endif
