#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include "error_functions.h"

error_functions::error_functions()
{
}

error_functions::~error_functions()
{
}

// note, stay within +/- 11.0 because Erfi[11] = 1.8e51
// which is getting close to limits on IEEE double
// can only do orders up to 148 to get that accurate
// because 11e(2*148) is pushing IEEE double too. 
double error_functions::erfi(double x, int order)
{
  int    i;
  double ans   = 0.0;
  double powx  = x;
  double sqrdx = x*x;
  double fac1  = 1.0;
  double fac2  = 1.0;

  for (i=0; i<order; i++)
  {
    ans  += (1.0/(fac1-fac2*0.5))*powx;
    fac2  = fac1;
    fac1 *= ((double) (i+2.0));
    powx *= sqrdx;
  }

  ans *= 1.0/sqrt(vnl_math::pi);
  return(ans);
}
