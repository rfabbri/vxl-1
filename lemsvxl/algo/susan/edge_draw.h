#ifndef _EDGE_DRAW_H
#define _EDGE_DRAW_H
#include "susan.h"

struct Susanpixels *edge_draw(uchar *in, uchar *mid, int x_size, int y_size,int*a,int*b,uchar * gradient,char * angle_edges);

#endif
