/* misc_geometry.c -- Perry A. Stoll Wed May 22 1996
 *
 * Time-stamp: <96/05/22 18:20:16 pas>
 * $Log$ 
 */

#include <math.h>


/************************************************************
  @routine@ angle_difference
  
  @param@ angle1 - subtracand angle.
  @param@ angle2 - subtrahend angle.
  @param@ degrees_p - if true, angles are in degrees, otherwise in radians.
  
  @desc@ Compute the difference angle1 - angle2.

  This function deals with the following problem: the angles 10deg and
  350deg are not 340deg apart, rather we would like them to be -20deg
  apart.  e.g. 10,20==> 10; 40,20==>-20; 10,350==>-20; 350,10==>20
  @end@
  **/
double angle_difference(double angle1, double angle2,int degrees_p)
{
  const double max_change = degrees_p? 180.0 : M_PI;
  double max_value = 2 * max_change;
  double diff = fmod(angle1,max_value) - fmod(angle2,max_value);
  double val;

  if (fabs(diff) < max_change){
    val = diff;
  }
  else if (diff > 0){
    val = diff - max_value;
  }
  else{
    val = diff + max_value;
  }
  return val;
}

int is_almost_zero(double x)
{
  extern double eno_NearZeroValue;
  return fabs(x) < eno_NearZeroValue;
}

int is_almost_equal(double val1, double val2, double tolerance)
{
  return fabs(val1 - val2) < tolerance;
}

int is_in_interval(double p,double start,double end)
{
  return (p >= start) && (p <= end);
}


