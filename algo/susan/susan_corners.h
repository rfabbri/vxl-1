#ifndef _SUSAN_CORNERS_H
#define _SUSAN_CORNERS_H
#include "susan.h"

struct Susanpixels *susan_corners(uchar *in, int *r, uchar *bp, int max_no, CORNER_LIST corner_list,int x_size, int y_size);

#endif
