#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 *                                    *
 *       Copyright 19xx, Brown University, Providence, RI        *
 *                                    *
 *  Permission to use and modify this software and its documentation    *
 *  for any purpose other than its incorporation into a commercial    *
 *  product is hereby granted without fee. Recipient agrees not to    *
 *  re-distribute this software or any modifications of this        *
 *  software without the permission of Brown University. Brown        *
 *  University makes no representations or warrantees about the        *
 *  suitability of this software for any purpose.  It is provided    *
 *  "as is" without express or implied warranty. Brown University    *
 *  requests notification of any modifications to this software or    *
 *  its documentation. Notice should be sent to:            *
 *                                      *
 *  To:                                    *
 *        Software Librarian                        *
 *        Laboratory for Engineering Man/Machine Systems,        *
 *        Division of Engineering, Box D,                *
 *        Brown University                        *
 *        Providence, RI 02912                        *
 *        (401) 863-2118                        *
 *        Software_Librarian@lems.brown.edu                *
 *                                      *
 *  We will acknowledge all electronic notifications.            *
 *                                     *
 ************************************************************************/


#ifndef __GENO_H__
#define __GENO_H__

/************************************************************************
 *                                    *
 *    File:        geno.h                        *
 *     Project:    GENO Library                    *
 *    Author:     Kwun Han                    *
 *    Email:      kwunh+@cs.cmu.edu                *
 *            kwh@lems.brown.edu                *
 *    Date:        Summer '96                    *
 *                                    *
 ************************************************************************/

/*      $Id: dbseg_subpixel_geno.h,v 1.1 2009-06-09 22:29:18 fine Exp $    */

/* #include <tracer_types.h> */
#include "dbseg_subpixel_geno-types.h"
#if defined(__cplusplus)
extern "C" {
#endif


/************************************************************************
 * This takes in the array of point from Tek's code and creates a list
 * curves
 ************************************************************************/

  /*GENO_Curve_List*
    make_curves(ContourInfo* contours);*/
void
  geno_free_curve(GENO_Curve_List* curve);

GENO_Curve *
  geno_curve_new(int npoints,int closed);
void
  geno_curve_delete(GENO_Curve *lc);

GENO_Curve *
  geno_curve_alloc_fields(GENO_Curve *lc,
              int npoints,int closed);
void
  geno_curve_dealloc_fields(GENO_Curve *lc);

GENO_Interpolated_Curve*
  geno_make_intervals(GENO_Curve_List* curves);

  /*GENO_Interpolated_Curve *
    geno_compute(ContourInfo* contours);*/
GENO_Interpolated_Curve *
  geno_intervals_from_curves(GENO_Curve_List *curves);
void
  geno_free_interpolated_curve(GENO_Interpolated_Curve* icurve);

#if defined(__cplusplus)
}
#endif
#endif

#ifdef __cplusplus
}
#endif

