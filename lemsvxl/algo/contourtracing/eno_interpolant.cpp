/* eno_intervals.c  -- Perry A. Stoll Wed May 22 1996
 *
 * Time-stamp: <96/07/08 15:51:36 pas>
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2003/11/12 22:01:36  mcchang
 * MC..finished. it compiles
 *
 * Revision 1.1  2003/10/29 15:32:50  mcchang
 * Ming
 *
 * Revision 1.1.1.1  2003/03/31 18:17:08  mcchang
 * no message
 *
 */

#ifndef lint
static char *rcsid = "$Header: /vision/projects/cvsroot2svn/cvsroot/lemsvxlsrc/algo/contourtracing/eno_interpolant.cpp,v 1.3 2004-10-15 16:37:28 mleotta Exp $";
#endif

#include <stdio.h>
#include <vcl_cmath.h>
#include <limits.h>

#include "1d_eno_types.h"



/************************************************************
  @routine@ enointerp_derivative_at
  
  @param@ interp - the eno interpolant for this interval.
  @param@ loc - location where we are computing derivative.

  @desc@ Evaluate the derivative of the eno interpolant at the given location.

  @return@ double - value of derivative.
  @end@
************************************************************/
double
enointerp_derivative_at(ENO_Interpolant_t const *interp,double loc)
{
  double const
      a2 = interp->poly.coeff[kSecondOrderIndex],
      a1 = interp->poly.coeff[kFirstOrderIndex];
  return  (2.0 * a2 * loc) + a1;
}


/************************************************************
  @routine@ enointerp_compute_polynomial
  
  @param@ interp - the eno interpolant for this interval.
  @param@ x_loc - location where we are interpolating this ENO polynomial.
  @param@ data - local array of data from which to choose when
  performing the ENO interpolation. data[1] should be the data at
  x=x_loc.

  @desc@ Compute the ENO polynomial coefficients for a local
  neighborhood of data at a given point. Currently, limited to 2nd
  order ENO.

  @return@ int - not used.
  @end@
************************************************************/
int
enointerp_compute_polynomial(ENO_Interpolant_t *interp,
                             double x_loc, 
                             double const data[kENODataLength])
{
  double
    a2,    /* 2nd order coefficient of forward polynomial */
    b2,    /* 2nd order coefficient of backward polynomial */
    c2;    /* 2nd order coefficient in choosen direction */
  int const
    off = 1;    /* offset in data array of x_loc entry */

  dbg_leave("enointerp_compute_polynomial");
    
  /* compute leading coefficient of forward and backward polynomials */
  a2 = (data[off+2] - 2.0* data[off+1] + data[off+0])/2.0;
  b2 = (data[off+1] - 2.0* data[off+0] + data[off-1])/2.0;

  /* determine which direction to use for interpolation */
  interp->forward = fabs(a2) < fabs(b2);
    
  /* choose polynomial with smaller variation, where variation is
     measured as absolute value of leading polynomial coefficient.*/
  c2 = interp->forward ? a2:b2;
    
  /* compute and store all polynomial coefficients for this interpolant */
  interp->poly.coeff[kSecondOrderIndex] = c2;
  interp->poly.coeff[kFirstOrderIndex] = data[off+1] - (c2*(2*x_loc+1) + data[off+0]);
  interp->poly.coeff[kZeroOrderIndex] =
    (data[off+0] + c2*x_loc*(x_loc+1)) - (x_loc*(data[off+1] - data[off+0]));

  dbg_leave("enointerp_compute_polynomial");
  return 0;
}


/************************************************************
  @routine@ enointerp_compute_geometric_values
  
  @param@ interp - the eno interpolant for this interval.

  @desc@ Compute the tangent and curvature values at start and end of
  interval. Must previously have computed coefficients.

  @return@ int - not used.
  @end@
************************************************************/
int
enointerp_compute_geometric_values(ENO_Interpolant_t *interp)
{
  double
    a,b,      /* coefficients of polynomial */
    dy_dx;      /* derivative of polynomial at point loc */
  extern double angle_difference(double,double,int);

  dbg_enter("enointerp_compute_geometric_values");

  a = interp->poly.coeff[kSecondOrderIndex];
  b = interp->poly.coeff[kFirstOrderIndex];

  /* do values at start of interval  */
  dy_dx =  enointerp_derivative_at(interp,interp->start);
  interp->start_tangent = atan2(1.0,dy_dx)*kDegreesPerRadian;
  interp->start_curvature = 2.0*a / pow(1.0 + dy_dx*dy_dx,1.5);


  /* do values at end of interval  */
  dy_dx =  enointerp_derivative_at(interp,interp->end);
  interp->end_tangent = atan2(1.0,dy_dx)*kDegreesPerRadian;
  interp->end_curvature = 2.0*a / pow(1.0 + dy_dx*dy_dx,1.5);

  /* compute total curvature across this interpolant interval */
  interp->total_curvature = angle_difference(interp->end_tangent,
                                             interp->start_tangent,1);


  dbg_leave("enointerp_compute_geometric_values");
  return 0;
}


/************************************************************
  @routine@ enointerp_make_interpolant
  
  @param@ interp - the eno interpolant to compute values for.

  @param@ start - starting location of this interval.

  @param@ end - ending location of this interval.

  @param@ data - local neighborhood of values that are considered when
  interpolating the ENO polynomial.

  @desc@ Factory method for interpolant structure, initializing and
  computing all fields.

  @return@ int - not used.
  @end@
************************************************************/
int
enointerp_make_interpolant(ENO_Interpolant_t *interp,
         double start,double end,
         double const data[kENODataLength])
{
  dbg_enter("enointerp_make_interpolant");

  interp->start = start;
  interp->end   = end;
  enointerp_compute_polynomial(interp,start,data);
  enointerp_compute_geometric_values(interp);

  dbg_leave("enointerp_make_interpolant");

  return 0;
}


int enointerp_print(ENO_Interpolant_t *interp,FILE *fp)
{
  ENO_ZeroCrossing_t zeros;
  int n_zeros;
  
  n_zeros = enozerox_from_polynomial(&zeros,&interp->poly);
  
  fprintf(fp,"(start,end) = (%g,%g)\n",interp->start,interp->end);
  fprintf(fp,"  forward_p = %s\n",interp->forward?"true":"false");
  fprintf(fp,"  poly coeffs = (%.10g,%.10g,%.10g)\n",
    interp->poly.coeff[kSecondOrderIndex],
    interp->poly.coeff[kFirstOrderIndex],
    interp->poly.coeff[kZeroOrderIndex]);
  fprintf(fp,"  poly zeros = ");
  enozerox_print(&zeros,fp);
  fprintf(fp,"  tangents(deg) = (%5.5g , %5.5g)\n",
    interp->start_tangent,interp->end_tangent);
  fprintf(fp,"  curvature = (%5.5g , %5.5g)\n",
    interp->start_curvature,interp->end_curvature);
  fprintf(fp,"  total_curvature(deg)=%5.5g\n", interp->total_curvature);
  
  return ferror(fp);
}



