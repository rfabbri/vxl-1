#ifndef _FUZZY_BOOLEAN_H
#define _FUZZY_BOOLEAN_H

#include <vcl_cmath.h>
#include "defines.h"

// This file contains Boolean functions that are defined
// for comparing inaccurate quantities (with known relative errors)
// with absolute certainty.

// In other words the returned bool value from these functions can
// be trusted provided that the relative errors were correctly prescribed


//##########################################################
// RELATIVE ERROR
// SEE "float.h"

/////////////////  THE DOUBLE VERSION //////////////////////

inline double _epsilon (double a, double b, double relativeEpsilon)
{
  //double epsilon = a*relativeEpsilon + b*relativeEpsilon;
  double epsilon = (a+b)*relativeEpsilon;

  //If epsilon too small, use reasonable fix one!
  if (epsilon < relativeEpsilon)
    epsilon = relativeEpsilon;
  return epsilon;
}

inline bool _isEq (double a, double b, double relativeEpsilon)
{
  double epsilon = _epsilon (a, b, relativeEpsilon);
  bool result = (vcl_fabs((a)-(b))<=epsilon?1:0);

  //if (MessageOption >= MSG_NORMAL)
  //if (result != (a==b)) {
  //  COUT<< "FUZZY '=' Epsilon="<<epsilon<< ", Error="<< vcl_fabs((a)-(b))<<endl;
  //  MSGOUT(2);
  //}
  return result;
}

inline bool _isLEq (double a, double b, double relativeEpsilon)
{
  if ( _isEq(a,b,relativeEpsilon) )
    return true;
  else
    return (a<b);
}

inline bool _isGEq (double a, double b, double relativeEpsilon)
{
  if ( _isEq(a,b,relativeEpsilon) )
    return true;
  else
    return (a>b);
}

inline bool _isL (double a, double b, double relativeEpsilon)
{
  double epsilon = _epsilon (a, b, relativeEpsilon);
  bool result = ((a)+epsilon<(b)?1:0);

  //if (MessageOption >= MSG_NORMAL)
  //if (result != (a<b)) {
  //  COUT<< "FUZZY '<' Epsilon="<<epsilon<< ", Error="<< b-a <<endl;
  //  MSGOUT(2);
  //}
  return result;
}

inline bool _isG (double a, double b, double relativeEpsilon)
{
  double epsilon = _epsilon (a, b, relativeEpsilon);
  bool result = ((a)>(b)+epsilon?1:0);

  //if (MessageOption >= MSG_NORMAL)
  //if (result != (a>b)) {
  //  COUT<< "FUZZY '>' Epsilon="<<epsilon<< ", Error="<< a-b <<endl;
  //  MSGOUT(2);
  //}
  return result;
}

// !!WORK!!
//THE ROUND FUNCTION IS NOT ACTUALLY ROUNDING THINGS!!!
inline void _round (float& value, double epsilon)
{
  double ep = 1/epsilon;
  value = (float) ( vcl_floor((double)value*ep+0.5)/ep);

  //int i;
  //ftol(value/epsilon,&i); return((double)i);
}

//THE ROUND FUNCTION IS NOT ACTUALLY ROUNDING THINGS!!!
inline void _round (double& value, double epsilon)
{
  double ep = 1/epsilon;
  value = ( vcl_floor (value*ep+0.5))/ep;

  //int i;
  //ftol(value/epsilon,&i); return((double)i);
}


//##########################################################
// THE ANGLE DEFINITIONS
//##########################################################

inline bool _isEqAngle (double a, double b, double relativeEpsilon)
{
  if (_isEq(a, M_PI*2, relativeEpsilon))
    a = a-M_PI*2;
  if (_isEq(b, M_PI*2, relativeEpsilon))
    b = b-M_PI*2;

  double epsilon = _epsilon (a, b, relativeEpsilon);
  bool result = (vcl_fabs((a)-(b))<=epsilon?1:0);

  return result;
}

#endif
