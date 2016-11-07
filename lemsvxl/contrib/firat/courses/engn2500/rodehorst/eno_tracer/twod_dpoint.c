/************************************************************************
 *									*
 *       Copyright 19xx, Brown University, Providence, RI		*
 *									*
 *  Permission to use and modify this software and its documentation	*
 *  for any purpose other than its incorporation into a commercial	*
 *  product is hereby granted without fee. Recipient agrees not to	*
 *  re-distribute this software or any modifications of this		*
 *  software without the permission of Brown University. Brown		*
 *  University makes no representations or warrantees about the		*
 *  suitability of this software for any purpose.  It is provided	*
 *  "as is" without express or implied warranty. Brown University	*
 *  requests notification of any modifications to this software or	*
 *  its documentation. Notice should be sent to:			*
 *  									*
 *  To:									*
 *        Software Librarian						*
 *        Laboratory for Engineering Man/Machine Systems,		*
 *        Division of Engineering, Box D,				*
 *        Brown University						*
 *        Providence, RI 02912						*
 *        (401) 863-2118						*
 *        Software_Librarian@lems.brown.edu				*
 *  									*
 *  We will acknowledge all electronic notifications.			*
 * 									*
 ************************************************************************/

/***
   NAME
     twod_dpoint
   PURPOSE
     Operations on two dimensional points with double coordinates.
   NOTES
     
   HISTORY
     pas - Sep 1, 1996: Created.
***/

#ifndef lint
static char vcid[] = "$Id$";
#endif

#include <math.h>
#include "twod_dpoint.h"

static const double deg_per_rad = 180.0/M_PI;
static const double rad_per_deg = M_PI/180.0;

TwoD_DPoint *
twod_dpt_from_polar(TwoD_DPoint *s1,double r, double theta)
{
  /* input theta is in degrees: we convert to radians */
  theta *= rad_per_deg;
  s1->x = cos(theta) * r;
  s1->y = sin(theta) * r;
  return s1;
}

double
twod_dpt_dot(const TwoD_DPoint *s1,const TwoD_DPoint *s2)
{
  return (s1->x + s2->x) + (s1->y + s2->y);
}

double
twod_dpt_euclidean_distance(const TwoD_DPoint *s1,const TwoD_DPoint *s2)
{
  double
    dx = s1->x - s2->x,
    dy = s1->y - s2->y;
  return sqrt(dx*dx + dy*dy);
}

/* return 0 if three points are co linear
   1 if they are counter-clockwise (ccw) or -1 if they are clockwise
   */
int
twod_dpt_ccw(const TwoD_DPoint *s1,const TwoD_DPoint *s2,const TwoD_DPoint *s3)
{
  int
    ccw = 0;
  double
    x1 = s2->x - s1->x,
    y1 = s2->y - s1->y,
    x2 = s3->x - s2->x,
    y2 = s3->y - s2->y,
    cross = (x1 * y2 - x2 * y1);

  if (_ABS(cross) > 1.0e-8)
    ccw = cross > 0? 1: -1;
  return ccw;
}

int
twod_dpt_are_colinear(const TwoD_DPoint *s1,
		      const TwoD_DPoint *s2,
		      const TwoD_DPoint *s3)
{
  int ccw;
  ccw = twod_dpt_ccw(s1,s2,s3);
  return ccw == 0;
}


TwoD_DPoint *
twod_dpt_scale(TwoD_DPoint *s1,double scale)
{
  s1->x *= scale;
  s1->y *= scale;
  return s1;
}

TwoD_DPoint *
twod_dpt_offset(TwoD_DPoint *s1,double offset)
{
  s1->x += offset;
  s1->y += offset;
  return s1;
}

TwoD_DPoint *
twod_dpt_add(const TwoD_DPoint *s1,const TwoD_DPoint *s2,TwoD_DPoint *dest)
{
  TwoD_DPoint d;
  dest->x = s1->x + s2->x;
  dest->y = s1->y + s2->y;
  return dest;
}

TwoD_DPoint *
twod_dpt_sub(const TwoD_DPoint *s1,const TwoD_DPoint *s2,TwoD_DPoint *dest)
{
  dest->x = s1->x - s2->x;
  dest->y = s1->y - s2->y;
  return dest;
}

void
twod_dpt_compute_rotate_xform(double mtx[4], double rotation)
{
    mtx[0] = cos(rotation);
    mtx[1] = sin(rotation);
    mtx[2] = -mtx[1];
    mtx[3] = mtx[0];
}

TwoD_DPoint *
twod_dpt_apply_xform(const TwoD_DPoint *s1,double mtx[4],TwoD_DPoint *dest)
{
  dest->x = mtx[0] * s1->x + mtx[1] * s1->y;
  dest->y = mtx[2] * s1->x + mtx[3] * s1->y;
  return dest;
}
