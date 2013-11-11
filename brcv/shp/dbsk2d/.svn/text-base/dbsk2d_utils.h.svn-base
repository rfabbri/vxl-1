// This is brcv/shp/dbsk2d/dbsk2d_utils.h
#ifndef dbsk2d_utils_h_
#define dbsk2d_utils_h_
//:
// \file
// \brief This file contains various useful functions 
// for use internal to this library.
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
// \endverbatim

#include <dbsk2d/dbsk2d_defines.h>
#include <dbsk2d/dbsk2d_assert.h>
#include <dbsk2d/dbsk2d_exception.h>
#include <dbsk2d/dbsk2d_fuzzy_boolean.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

//: is z between x and y ?
inline bool isBetween(double z, double x, double y)
{
  return ((z<=x && z>=y) || (z>=x && z<=y));
}

//: is z between x and y (x<=z<=y)(with A_EPSILON fuzziness) ?
inline bool AisBetween(double z, double x, double y)
{
  return AisGEq(z,x) && AisLEq(z,y);
}

inline void swap (double& v1, double& v2)
{
   double temp= v2;
   v2 = v1;
   v1 = temp;
}

// Functions to deal with angles

//: Convert an angle to [0, 2Pi) range
inline double angle0To2Pi (double angle)
{
  double a;
  if (angle>=2*vnl_math::pi)
    a = vcl_fmod (angle,vnl_math::pi*2);
  else if (angle < 0)
    a = (2*vnl_math::pi+ vcl_fmod (angle,2*vnl_math::pi));
  else 
    a= angle;

  // added by Nhon: these two lines of code is to fix the bug when
  // angle = -1.1721201390607859e-016
  // then after all the computation, we get
  // a = 6.2831853071795862 == 2*vnl_math::pi !!!!!!!
  // the only case this can happen is when a is very close to zero.

  if (!(a>=0 && a<2*vnl_math::pi)) {
    a = 0;
  }

  // dbsk2d_assert (a>=0 && a<2*vnl_math::pi);
  return a;
}

//: Convert an angle to a Fuzzy [0, 2Pi)
inline double angle0To2PiFuzzy (double angle)
{
  double a = angle0To2Pi(angle);

  if (_isEq (a, 0, A_EPSILON))
    return 0;
  else if (_isEq (a, vnl_math::pi*2, A_EPSILON))
    return 0;
  else
    return a;
}

inline double angle_mPiToPi(double a) {
  if (a < -vnl_math::pi)
    return a+2*vnl_math::pi;
  else if (a > vnl_math::pi)
    return a-2*vnl_math::pi;
  else
    return a;
}

//: compute the Counterclockwise angle between the reference and the angle
inline double CCW (double reference, double angle)
{
   double fangle = angle0To2Pi(angle);
   double fref = angle0To2Pi(reference);

   if (fref > fangle){
    return angle0To2Pi(2*vnl_math::pi - (fref - fangle));
  }
   else
      return angle0To2Pi(fangle - fref); 
}

//: Is v between start and end traversing CCW? (EndPoint NOT Included)
inline bool _validStartEnd0To2Pi (double v, double start, double end)
{
  //dbsk2d_assert (start!=end);
  //dbsk2d_assert (v>=0 && v<=2*vnl_math::pi);
  //dbsk2d_assert (v>=0 && v<=2*vnl_math::pi);
  //dbsk2d_assert (v>=0 && v<=2*vnl_math::pi);

  //1)Normal case:
   if ( end>v && v>start ) 
      return true;

  //2)0-2*vnl_math::pi crossing case:
   if (end<start)
      if ((v>=0 && v<end) || (v>start && v<=2*vnl_math::pi))
         return true;

   return false; 
}

//: Is v between start and end traversing CCW? (EndPoint Included)
// \todo _validStartEnd0To2Pi needs to be fuzzy too.
inline bool _validStartEnd0To2PiEPIncld (double v, double start, double end)
{
  if (_isEq (v, start, A_EPSILON) || _isEq (v, end, A_EPSILON))
    return true;
  if (_isEq (v, 2*vnl_math::pi, A_EPSILON) && _isEq (start, 0, A_EPSILON))
    return true;
  if (_isEq (v, 0, A_EPSILON) && _isEq (start, 2*vnl_math::pi, A_EPSILON))
    return true;
  if (_isEq (v, 2*vnl_math::pi, A_EPSILON) && _isEq (end, 0, A_EPSILON))
    return true;
  if (_isEq (v, 0, A_EPSILON) && _isEq (end, 2*vnl_math::pi, A_EPSILON))
    return true;

   return _validStartEnd0To2Pi (v, start, end);
}

inline void _round (float& value, double epsilon)
{
  double ep = 1/epsilon;
  value = (float) ( vcl_floor((double)value*ep+0.5)/ep);

  //int i;
  //ftol(value/epsilon,&i); return((double)i);
}

inline void _round (double& value, double epsilon)
{
  double ep = 1/epsilon;
  value = ( vcl_floor (value*ep+0.5))/ep;

  //int i;
  //ftol(value/epsilon,&i); return((double)i);
}

inline void Bround (INPUT_TYPE& value) { _round (value, B_EPSILON); }
inline void Bround (double& value) { _round (value, B_EPSILON); }

inline double _angle_vector_dot( double angle1, double angle2)
{
  return vcl_cos(angle1)*vcl_cos(angle2)+vcl_sin(angle1)*vcl_sin(angle2);
}

// include other utils
#include <dbsk2d/dbsk2d_geometry_utils.h>

#endif //dbsk2d_utils_h_
