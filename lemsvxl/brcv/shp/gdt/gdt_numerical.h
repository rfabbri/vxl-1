//: Aug 19, 2005 MingChing Chang
//  Numerical Definitions for Goedesic DT Wavefront Propagation Algorithm

#ifndef gdt_numerical_h_
#define gdt_numerical_h_

#include <vcl_cmath.h>
#include <vcl_utility.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>


///#######################################
//: optimization switches

//should turn this off for a release build.
#define GDT_DEBUG_MSG         1

//: To separate the design for each algorithm.
//  comment to turn off, uncomment to turn on
#define GDT_ALGO_I            0
#define GDT_ALGO_F            1
#define GDT_ALGO_FS           0
#define GDT_ALGO_WS           1

#define GDT_METHOD_I          1
#define GDT_METHOD_F          2
#define GDT_METHOD_FS         3
#define GDT_METHOD_WS         4

///#######################################

#define GDT_HUGE              100000
#define GDT_NEG_HUGE          -100000
#define GDT_INVALID_HUGE      200000

#define GDT_FINE_EPSILON      5E-15
#define GDT_MID_EPSILON       5E-10
#define GDT_TAU_EPSILON       1E-6
#define GDT_DIST_EPSILON      1E-5

//: used in the face-based propagation.
#define GDT_TAU_EPSILON_DIV   1E-7  

///#######################################

// this value is reserved for the invalid solution.
#define INVALID_TAU           -100000
#define CONTACT_SHOCK_INT_TAU -50000

//Need to be negative for the interval propagation!
#define INVALID_DIST          GDT_NEG_HUGE

#define is_between(z,x,y) ((((z)<=(x) && (z)>=(y))||((z)>=(x) && (z)<=(y)))?(1):(0))

///#######################################
//: the very fine equality test, use absolute test.
inline bool _eqF (const double a, const double b)
{
  return vcl_fabs (a-b) < GDT_FINE_EPSILON;
}

inline bool _leqF (const double a, const double b)
{
  return a < b || _eqF (a, b);
}

///#######################################
//: the mid-epsilon equality test, use absolute test.
inline bool _eqM (const double a, const double b)
{
  return vcl_fabs (a-b) < GDT_MID_EPSILON;
}

inline bool _leqM (const double a, const double b)
{
  return a < b || _eqM (a, b);
}

inline bool _lessM (const double a, const double b)
{
  return (a+GDT_MID_EPSILON < b);
}

///#######################################
//: the equality test for angle
//  the arc length parameter to describe intervals on edges.
inline bool _eqT (const double a, const double b)
{
  return vcl_fabs (a-b) < GDT_TAU_EPSILON;
}

inline bool _leqT (const double a, const double b)
{
  return a < b || _eqT (a, b);
}

inline bool _lessT (const double a, const double b)
{
  return (a+GDT_TAU_EPSILON < b);
}

///#######################################
//: the equality test for distance
inline bool _eqD (const double a, const double b)
{
  //For distance, use relative epsilon.
  const double er = (a+b)* GDT_DIST_EPSILON;
  return vcl_fabs (a-b) <= er;
}

inline bool _leqD (const double a, const double b)
{
  return a < b || _eqD (a, b);
}

//: the equality test for distance, given pre-computed epsilon
inline bool _eqD (const double a, const double b, const double e)
{
  const double er = (a+b)* e;
  return vcl_fabs (a-b) <= er;
}

inline bool _lessD (const double a, const double b)
{
  const double er = (a+b)* GDT_DIST_EPSILON;
  return (a+er < b);
}

inline void _swap (double& a, double& b)
{
  double temp = a;
  a = b;
  b = temp;
}

///#######################################
//: the equality test for 3D points

inline bool _eqT_PT (const vgl_point_3d<double> pt1, const vgl_point_3d<double> pt2)
{
  return _eqT(pt1.x(), pt2.x()) && _eqT(pt1.y(), pt2.y()) && _eqT(pt1.z(), pt2.z());
}


///#######################################
//: angle and vector CCW

inline double atan2_vector (vgl_point_2d<double> spt, vgl_point_2d<double> ept)
{
  return atan2 ( (double)ept.y() - (double)spt.y(), (double)ept.x() - (double)spt.x() );
}

inline double vector_minus_pi_to_pi (double vector)
{
  if (vector > vnl_math::pi)
    vector -= vnl_math::pi*2;
  else if (vector < -vnl_math::pi)
    vector += vnl_math::pi*2;

  // Do it twice to eliminate numerical issues
  if (vector > vnl_math::pi)
    vector -= vnl_math::pi*2;
  else if (vector < -vnl_math::pi)
    vector += vnl_math::pi*2;

  assert (-vnl_math::pi < vector && vector <= vnl_math::pi);
  return vector;
}

inline double angle_0_to_2pi (double angle)
{
  assert (angle < vnl_math::pi*2);

  if (angle >= vnl_math::pi*2)
    angle -= vnl_math::pi*2;
  else if (angle < 0)
    angle += vnl_math::pi*2;

  // Do it twice to eliminate numerical issues
  if (angle >= vnl_math::pi*2)
    angle -= vnl_math::pi*2;
  else if (angle < 0)
    angle += vnl_math::pi*2;

  assert (0 <= angle && angle < vnl_math::pi*2);
  return angle;
}

inline double CCW_angle (double ref_vector, double input_vector)
{
  assert (-vnl_math::pi < ref_vector && ref_vector <= vnl_math::pi);
  assert (-vnl_math::pi < input_vector && input_vector <= vnl_math::pi);

  return angle_0_to_2pi (input_vector - ref_vector);
}

#endif



