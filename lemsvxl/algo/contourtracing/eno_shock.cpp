/* eno_shock_intervals.c -- Perry A. Stoll Wed May 22 1996
 *
 * Time-stamp: <96/07/08 15:35:00 pas>
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2003/11/12 22:01:36  mcchang
 * MC..finished. it compiles
 *
 * Revision 1.1  2003/10/29 15:32:49  mcchang
 * Ming
 *
 * Revision 1.1.1.1  2003/03/31 18:17:10  mcchang
 * no message
 * 
 */

#ifndef lint
static char *rcsid = "$Header: /vision/projects/cvsroot2svn/cvsroot/lemsvxlsrc/algo/contourtracing/eno_shock.cpp,v 1.3 2004-10-15 16:37:28 mleotta Exp $";
#endif

#include <stdio.h>
#include <vcl_cmath.h>

#include "1d_eno_types.h"

/* tangent change in degrees which indicates a shock */
double enoshock_tangent_change_threshold = 20.0;

/* curvature change which indicates a shock*/
double enoshock_curvature_change_threshold = 0.3;


/************************************************************
  @routine@ enoshock_make_shock

  @param@ prev - interval behind the interval which receives the shock.

  @param@ cur - interval which receives the shock.

  @param@ next - interval beyond the interval which receives the shock.

  @desc@ This routine places a shock at the current interval. The
  properties of the shock, currently just its location, are based the
  previous and next intervals. The intersections of the polynomials of
  the previous and next intervals determines the location of the shock.

  @return@ int - returns the number of roots found in the polynomial
  which is the intersection of the polynomials of the previous and
  next intervals. 


  @end@
************************************************************/
int
enoshock_make_shock(ENO_Interval_t *prev,ENO_Interval_t *cur,ENO_Interval_t *next)
{
  int
      valid_root_cnt = 0;       /* number of real roots in the difference
                                   polynomial */
  ENO_Polynomial_t
      diff_poly;
  ENO_ZeroCrossing_t
      zeros;
  
  
  dbg_enter("enoshock_place_shock");

  diff_poly.coeff[kSecondOrderIndex] =
      prev->interp.poly.coeff[kSecondOrderIndex]
      - next->interp.poly.coeff[kSecondOrderIndex];
  diff_poly.coeff[kFirstOrderIndex] =
      prev->interp.poly.coeff[kFirstOrderIndex]
      - next->interp.poly.coeff[kFirstOrderIndex];
  diff_poly.coeff[kZeroOrderIndex] =
      prev->interp.poly.coeff[kZeroOrderIndex]
      - next->interp.poly.coeff[kZeroOrderIndex];

  valid_root_cnt = enozerox_from_polynomial_in_interval(&zeros,&diff_poly,
                                                        cur->interp.start,
                                                        cur->interp.end);
  
  /* if we found valid roots, create a new shock structure */
  if (valid_root_cnt > 0){

      /* it doesn't seem like we should have shocks in successive
         intervals - will see if this ever occurrs. */
    if (prev->shock && 0){
      /* XXX: keep only the shock with the greater curvature */
      /*if (prev->interp.poly.coeff[kSecondOrderIndex] < 
    cur->interp.poly.coeff[kSecondOrderIndex]){
  enoDEL(prev->shock);
  prev->shock = NULL;
  printf("removing shock from interval (%f,%f)",
         prev->interp.start,prev->interp.end);
      }
      */
      printf("**********placing shock at interval (%f,%f)",
       cur->interp.start,cur->interp.end);
      printf("but neighbor already has a shock.\n**** does that make sense?***\n");
    }
      
      /* we are only using the first root found. Maybe we should do
         something different if there are two?  */
      if (valid_root_cnt == 2){
          dmessage2("two real roots, ignoring root at %.5g...\n",zeros.loc[1]);
      }

      if (cur->shock == NULL){
          cur->shock = enoNEW(ENO_Shock_Interpolant_t,1);
      }
      
      cur->shock->shock_loc = zeros.loc[0];
      enointerp_copy(&cur->shock->backward,&prev->interp);
      enointerp_copy(&cur->shock->forward,&next->interp);
  }

  dbg_leave("enoshock_make_shock");
  return valid_root_cnt;
}



int
enoshock_check_for_shock(ENO_Interval_t *prev,ENO_Interval_t *cur,ENO_Interval_t *next)
{
  double
      delta_tan,                /* tangent change within interval */
      delta_tan_forward,        /* holds difference of tangent between
                                   next and current interval in
           degrees.  */
      delta_tan_backward,  /* same as above for cur and previous */
      delta_curv,               /* curvature change within interval */
      delta_curv_forward,  /* curvature change between next and
           cur interval.  */
      delta_curv_backward;  /* same as above for cur and previous */
  int
      large_tangent_change_p,  /* true if tangent change is large */
      large_curvature_change_p,  /* true if curvature change is large */
      need_shock_p=0;    /* true if shock detected.  */
  
  /* compute changes in tangents and curvatures between successive
     intervals. large changes indicate a shock should be placed. */
  
  /** Why doesn't Kaleem's original code consider the change within
    an interval? This seems to be the only way to capture shocks in
      certain configurations...i.e.
      
      0.   1.   4.   9.  16.  16.   9.   4.   1.   0.
      
      To place a shock correctly between the 16s, you need to look
      at the change in tangent _within_ an interval. I've added it
      here.  -Perry
      **/
  
  /* only look to place shocks in regions of locally maximum curvature */
  if ( (fabs(cur->interp.total_curvature) > fabs(prev->interp.total_curvature))
       || (fabs(cur->interp.total_curvature) > fabs(next->interp.total_curvature))){
      
      delta_tan = 
          angle_difference(cur->interp.end_tangent,
                           cur->interp.start_tangent,1);
      delta_tan_backward =
          angle_difference(cur->interp.start_tangent,
                           prev->interp.end_tangent,1);
      delta_tan_forward =
          angle_difference(next->interp.start_tangent,
                           cur->interp.end_tangent,1);
      
      /* set flag if either tangent exceeds the threshold */
      large_tangent_change_p =
          ((fabs(delta_tan_backward) > enoshock_tangent_change_threshold)
           || (fabs(delta_tan_forward) > enoshock_tangent_change_threshold));
      
      delta_curv =
          cur->interp.end_curvature - prev->interp.start_curvature;
      delta_curv_backward =
          cur->interp.start_curvature - prev->interp.end_curvature;
      delta_curv_forward =
          next->interp.start_curvature - cur->interp.end_curvature;
      
      /* set flag if either curvature exceeds the threshold */
      large_curvature_change_p =
          ((fabs(delta_curv_backward) > enoshock_curvature_change_threshold)
           || (fabs(delta_curv_forward) > enoshock_curvature_change_threshold));
      
      
      /* if only delta_tan would cause this to be marked a shock,
         print out for testing... */
      if (!large_tangent_change_p && !large_curvature_change_p
          && (fabs(delta_tan) > enoshock_tangent_change_threshold)){
          dmessage("\tADDING because of internal interval tangent change.\n");
          large_tangent_change_p = 1;
      }
      
      /* try to place a shock if tangent or curvature change is large */
      need_shock_p = (large_tangent_change_p || large_curvature_change_p);

      if (need_shock_p){

#ifdef DEBUG
          fprintf(stderr,"interval(%g,%g)\n",cur->interp.start,cur->interp.end);
          fprintf(stderr,"\tdelta tang =: (%5.5g,%5.5g,%5.5g)\n",
                  delta_tan_backward,delta_tan, delta_tan_forward);
          
          fprintf(stderr,"\tdelta curv =: (%5.5g,%5.5g,%5.5g)\n",
                  delta_curv_backward,delta_curv,delta_curv_forward);
#endif
          need_shock_p = enoshock_make_shock(prev,cur,next);
#ifdef DEBUG
          fprintf(stderr,"\n");
#endif
      }
  }

  return need_shock_p;
  
}

int enoshock_print(ENO_Shock_Interpolant_t *shock, FILE *fp)
{
    fprintf(fp,"shock_loc = %.10g\n",shock->shock_loc);
    return 0;
}

