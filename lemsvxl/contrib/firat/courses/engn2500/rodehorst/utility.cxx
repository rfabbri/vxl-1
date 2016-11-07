#include <math.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
using namespace std;

#include "utility.h"

/*****************************************************************************/
/* Utility Functions */

double Magnitude(const double v[DIM])
{
  // return the magnitude of the vector v
  return(sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]));
}




/*****************************************************************************/

double DotProduct(const double v1[DIM],
                  const double v2[DIM])
{
  // Return the do product out = v1.v2
  return(v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}




/*****************************************************************************/

void CrossProduct(const double v1[DIM],
                  const double v2[DIM],
                  double out[DIM])
{
   // Return the cross product out = v1 x v2
   out[0] = v1[1]*v2[2] - v1[2]*v2[1];
   out[1] = v1[2]*v2[0] - v1[0]*v2[2];
   out[2] = v1[0]*v2[1] - v1[1]*v2[0];
}




/*****************************************************************************/

void Normalize(double v[DIM])
{
  double d = Magnitude(v);
  if(d == 0.0)
  {
    cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << "Can't normalize zero-length vector." << endl;
    exit(1);
  }

  v[0] /= d;
  v[1] /= d;
  v[2] /= d;
}




/*****************************************************************************/

double Distance(const double v1[DIM],
                const double v2[DIM])
{
  // return the distance between v1 and v2,
  // in other words, the magnitude of v1-v2
  double a[DIM];
  a[0] = v1[0] - v2[0];
  a[1] = v1[1] - v2[1];
  a[2] = v1[2] - v2[2];
  return(Magnitude(a));
}




