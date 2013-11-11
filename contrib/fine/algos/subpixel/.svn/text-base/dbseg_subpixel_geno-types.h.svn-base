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


#ifndef __GENO_TYPES_H__
#define __GENO_TYPES_H__


/************************************************************************
 *                                    *
 *    File:        geno-types.c                    *
 *     Project:    GENO Library                    *
 *    Author:     Kwun Han                    *
 *    Email:      kwunh+@cs.cmu.edu                *
 *            kwh@lems.brown.edu                *
 *    Date:        Summer '96                    *
 *                                    *
 *     Description:                            *
 *                                    *
 *     This is the header file for the GENO interpolation code        *
 *     It defines the data structures and constants            *
 *                                     *
 ************************************************************************/

/*     $Id: dbseg_subpixel_geno-types.h,v 1.1 2009-06-09 22:29:18 fine Exp $    */

#include "dbseg_subpixel_1d_eno_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*     Neccessary constants and Macros */

#define MaxShocks (2)

#define PATCHSPAN(x) ((x) > 180. ? (x) - 360. : (((x) < -180.) ? (x) + 360. : x))
#define PATCHDEG(x) ((x) < 0 ? (x) + 360 : ((x) > 360) ? (x) - 360 : (x))
#define RAD2DEG(x) ((x) * 180 / M_PI)
#define DEG2RAD(x) ((x) * M_PI / 180)

/************************************************************************
 *  Structure holds whatever goes into a GENO arc
 * This holds the - locations
 *                - curvature
 *           - orientation
 ************************************************************************/

typedef struct {
    TwoD_DPoint start;
    TwoD_DPoint mid;
    TwoD_DPoint end;

    TwoD_DPoint center;
  
/*    double orientation;        orientation of tangent */
    double curvature;        /* curvature of this arc   *
                 * radius is 1 / curvature */
    int ccw;            /* boolean flag TRUE if this arc is ccw 
                   from start to finish - stored for efficiency */
} GENO_Arc;

typedef struct {
    TwoD_DPoint location;
    double speed;
    double direction;
    double tangent1;
    double tangent2;
    double time;
} GENO_Shock;

#define genoIntervalTypeDeadBit 0x100
#define genoIntervalTypeBoth 0
#define genoIntervalTypeLower 1
#define genoIntervalTypeUpper 2
#define genoIntervalTypeLinear 3

typedef struct {
    char type;            /* middle (3) upper (2), lower (1), both (0); *
                 * dead 0x100 alive 0x000          */
    
    char upper;            /* upper (1) or lower (0) arcs */
    TwoD_DPoint pts[4];
    GENO_Arc fit;        /* fit corresponds to eno fit, no shocks yet */
    int num_shocks;
    double begin_tangent;
    double end_tangent;
    GENO_Shock shocks[MaxShocks];
} GENO_Interval;

#define GENO_INTERVAL_DEAD(intv) (intv->type >> 4)

typedef struct GENO_Interpolated_Curve_s {
    int num_intv;
    int closed;
    GENO_Interval* interval;
    struct GENO_Interpolated_Curve_s* next;
} GENO_Interpolated_Curve;

typedef struct {
    int num_points;
    int closed;
    TwoD_DPoint* points;
} GENO_Curve;

typedef struct GENO_Curve_List_s {
    GENO_Curve* curve;
    struct GENO_Curve_List_s* next;
} GENO_Curve_List;


#if defined(__cplusplus)
}
#endif
#endif /*  __GENO_TYPES_H__*/

#ifdef __cplusplus
}
#endif

