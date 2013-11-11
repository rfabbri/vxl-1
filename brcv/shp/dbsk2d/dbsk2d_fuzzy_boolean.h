// This is brcv/shp/dbsk2d/dbsk2d_fuzzy_boolean.h
#ifndef dbsk2d_fuzzy_boolean_h_
#define dbsk2d_fuzzy_boolean_h_
//:
// \file
// \brief This file contains Boolean functions that are defined
// for comparing inaccurate quantities (with known absolute or 
// relative errors) with absolute certainty.
//
// In other words the returned bool value from these functions can
// be trusted provided that the errors were correctly prescribed.
//
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
//
//   Amir Tamrakar 08/29/05      Created two sets of boolean operators: 
//                               those that work with absolute errors and those
//                               that work with relative errors
//
//                               Also changed Ais* & Lis* operators to work with 
//                               absolute errors. But Ris* operator still works with
//                               relative errors.
// \endverbatim

#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <dbsk2d/dbsk2d_defines.h>

//----------------------------------------------------------
// Fuzzy boolean operators (absolute errors)
//----------------------------------------------------------
inline bool _isEq (double a, double b, double epsilon)
{
  return(vcl_fabs(a-b)<=epsilon?1:0);
}

inline bool _isLEq (double a, double b, double epsilon)
{
  if ( _isEq(a,b,epsilon) )
    return true;
  else
    return (a<b);
}

inline bool _isGEq (double a, double b, double epsilon)
{
  if ( _isEq(a,b,epsilon) )
    return true;
  else
    return (a>b);
}

inline bool _isL (double a, double b, double epsilon)
{
  return ((a+epsilon<b)?1:0);
}

inline bool _isG (double a, double b, double epsilon)
{
  return  (a>b+epsilon?1:0);
}

//Basically the same as _isEq, but for angles
//deals with 0<->2Pi discontinuity issue.
//EPSILONISSUE: Handling the 0, 2*pi DISCONTINUTY
inline bool _isEqAngle (VECTOR_TYPE a, VECTOR_TYPE b, double epsilon) {
  return _isEq(a, b, epsilon) || _isEq(a,b-2*vnl_math::pi,epsilon) || _isEq(a-2*vnl_math::pi,b,epsilon);
}

//----------------------------------------------------------
// Fuzzy boolean operators (relative errors)
//----------------------------------------------------------

//: compute absolute epsilon from a relative epsilon for a pair of numbers
inline double _epsilon (double a, double b, double relativeEpsilon)
{
  //double epsilon = a*relativeEpsilon + b*relativeEpsilon;
  double epsilon = (a+b)*relativeEpsilon;

  //If epsilon too small, use reasonable fix one!
  if (epsilon < relativeEpsilon)
    epsilon = relativeEpsilon;
  return epsilon;
}

inline bool _isEqRel (double a, double b, double relativeEpsilon)
{
  double epsilon = _epsilon (a, b, relativeEpsilon);
  return (vcl_fabs((a)-(b))<=epsilon?1:0);
}

inline bool _isLEqRel (double a, double b, double relativeEpsilon)
{
  if ( _isEqRel(a,b,relativeEpsilon) )
    return true;
  else
    return (a<b);
}

inline bool _isGEqRel (double a, double b, double relativeEpsilon)
{
  if ( _isEqRel(a,b,relativeEpsilon) )
    return true;
  else
    return (a>b);
}

inline bool _isLRel (double a, double b, double relativeEpsilon)
{
  double epsilon = _epsilon (a, b, relativeEpsilon);
  return ((a)+epsilon<(b)?1:0);
}

inline bool _isGRel (double a, double b, double relativeEpsilon)
{
  double epsilon = _epsilon (a, b, relativeEpsilon);
  return  ((a)>(b)+epsilon?1:0);
}

//BOUNDARY COMPARISON EPSILON
inline bool BisEq (double a, double b) { return _isEq(a, b, B_EPSILON); }
inline bool BisLEq (double a, double b) { return _isLEq(a, b, B_EPSILON); }
inline bool BisGEq (double a, double b) { return _isGEq(a, b, B_EPSILON); }
inline bool BisL (double a, double b) { return _isL(a, b, B_EPSILON); }
inline bool BisG (double a, double b) { return _isG(a, b, B_EPSILON); }

//TAU & ANGLE EPSILON (POINTS & ARCS) 
inline bool AisEq (VECTOR_TYPE a, VECTOR_TYPE b) { return _isEq(a, b, A_EPSILON); }
inline bool AisLEq (VECTOR_TYPE a, VECTOR_TYPE b) { return _isLEq(a, b, A_EPSILON); }
inline bool AisGEq (VECTOR_TYPE a, VECTOR_TYPE b) { return _isGEq(a, b, A_EPSILON); }
inline bool AisL (VECTOR_TYPE a, VECTOR_TYPE b) { return _isL(a, b, A_EPSILON); }
inline bool AisG (VECTOR_TYPE a, VECTOR_TYPE b) { return _isG(a, b, A_EPSILON); }
//Basically the same as AisEq, but deals with 0<->2Pi continuity issue.
inline bool AisEq02Pi (VECTOR_TYPE a, VECTOR_TYPE b) {
  return _isEqAngle(a, b, A_EPSILON);
}

//LINES LENGTH EPSILON
inline bool LisEq (double a, double b) { return _isEq(a, b, L_EPSILON); }
inline bool LisLEq (double a, double b) { return _isLEq(a, b, L_EPSILON); }
inline bool LisGEq (double a, double b) { return _isGEq(a, b, L_EPSILON); }
inline bool LisL (double a, double b) { return _isL(a, b, L_EPSILON); }
inline bool LisG (double a, double b) { return _isG(a, b, L_EPSILON); }

//RADIUS COMPARISON EPSILON
//this is still relative epsilon
//inline bool RisEq (double a, double b) { return _isEqRel(a, b, R_EPSILON); }
//inline bool RisLEq (double a, double b) { return _isLEqRel(a, b, R_EPSILON); }
//inline bool RisGEq (double a, double b) { return _isGEqRel(a, b, R_EPSILON); }
//inline bool RisL (double a, double b) { return _isLRel(a, b, R_EPSILON); }
//inline bool RisG (double a, double b) { return _isGRel(a, b, R_EPSILON); }

inline bool RisEq (double a, double b) { return _isEq(a, b, R_EPSILON); }
inline bool RisLEq (double a, double b) { return _isLEq(a, b, R_EPSILON); }
inline bool RisGEq (double a, double b) { return _isGEq(a, b, R_EPSILON); }
inline bool RisL (double a, double b) { return _isL(a, b, R_EPSILON); }
inline bool RisG (double a, double b) { return _isG(a, b, R_EPSILON); }

#endif //dbsk2d_fuzzy_boolean_h_
