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
     
   NOTES
     
   HISTORY
     pas - Sep 1, 1996: Created.
***/

#ifndef _TWOD_DPOINT_H_
#define _TWOD_DPOINT_H_
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct{
  double x,y;
} TwoD_DPoint;

/* Point operations */
/* theta is in degrees */

TwoD_DPoint *
  twod_dpt_from_polar(TwoD_DPoint *s1,double r, double theta);
double
  twod_dpt_dot(const TwoD_DPoint *s1,const TwoD_DPoint *s2);
double
  twod_dpt_euclidean_distance(const TwoD_DPoint *s1,const TwoD_DPoint *s2);
int
  twod_dpt_ccw(const TwoD_DPoint *s1,
	       const TwoD_DPoint *s2,
	       const TwoD_DPoint *s3);
int
  twod_dpt_are_colinear(const TwoD_DPoint *s1,
			const TwoD_DPoint *s2,
			const  TwoD_DPoint *s3);
TwoD_DPoint *
  twod_dpt_scale(TwoD_DPoint *s1,double scale);
TwoD_DPoint *
  twod_dpt_offset(TwoD_DPoint *s1,double offset);
TwoD_DPoint *
  twod_dpt_add(const TwoD_DPoint *s1,const TwoD_DPoint *s2,TwoD_DPoint *dest);
TwoD_DPoint *
  twod_dpt_sub(const TwoD_DPoint *s1,const TwoD_DPoint *s2,TwoD_DPoint *dest);
void
  twod_dpt_compute_rotate_xform(double mtx[4], double rotation);
TwoD_DPoint *
  twod_dpt_apply_xform(const TwoD_DPoint *s1,double mtx[4],TwoD_DPoint *dest);

#if defined(__cplusplus)
}
#endif
#endif /* TWOD_DPOINT */
