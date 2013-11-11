#ifndef _COMMON_H
#define _COMMON_H

#include <math.h>
#include "base_defines.h"
#include "base_I386Defines.h"
#include "base_FuzzyBoolean.h"

//##########################################################
// General

inline void swap (double& v1, double& v2)
{
   double temp;

   temp = v2;
   v2 = v1;
   v1 = temp;
}

inline double _dot (double x1, double y1, double x2, double y2)
{
  return x1*x2+y1*y2;
}

inline double _angle_vector_dot (double vector1, double vector2)
{
  //!!!SLOW
   return cos(vector1)*cos(vector2) + sin(vector1)*sin(vector2);
}

//##########################################################
// THE ANGLE DEFINITIONS
//##########################################################

inline double angle0To2Pi (double angle)
{
#if 0  
  while (angle >= M_PI*2)
    angle -= M_PI*2;
  while (angle < 0)
    angle += M_PI*2;
  return angle;
#else
  if (angle>2*M_PI)
      return fmod(angle,M_PI*2);
   else if (angle < 0)
      return (2*M_PI+fmod(angle,M_PI*2));
   else return angle;
#endif
}

inline double CCW (double reference, double angle1)
{
   double fangle1 = angle0To2Pi(angle1);
   double fref = angle0To2Pi(reference);

   if (fref > fangle1){
    return angle0To2Pi(2*M_PI - (fref - fangle1));
  }
   else
      return angle0To2Pi(fangle1 - fref); 
}

inline double aveAngle (double angle1, double angle2)
{
  double x1 = cos(angle1);
  double y1 = sin(angle1);
  double x2 = cos(angle2);
  double y2 = sin(angle2);
  double x = x1+x2;
  double y = y1+y2;
  double ave = atan2 (y1, x1);
  return angle0To2Pi (ave);
}

//EndPoint NOT Included
inline bool _validStartEnd0To2Pi (double v, double start, double end)
{
  ///float v = (float) dv;
  ///float start = (float) dstart;
  ///float end = (float) dend;

  //assert (start!=end);
  //assert (v>=0 && v<=2*M_PI);
  //assert (v>=0 && v<=2*M_PI);
  //assert (v>=0 && v<=2*M_PI);

  //1)Normal case:
   if ( end>v && v>start ) 
      return true;

  //2)0-2*M_PI crossing case:
   if (end<start)
      if ((v>=0 && v<end) || (v>start && v<=2*M_PI))
         return true;

   return false; 
}

//EndPoint Included
inline bool _validStartEnd0To2PiEPIncld (double v, double start, double end, double epsilon)
{
   if (_isEq(v, start, epsilon) || _isEq(v, end, epsilon))
    return true;

  if (_isEq(v, 2*M_PI, epsilon) && _isEq(start, 0, epsilon))
    return true;
  if (_isEq(v, 0, epsilon) && _isEq(start, 2*M_PI, epsilon))
    return true;
  if (_isEq(v, 2*M_PI, epsilon) && _isEq(end, 0, epsilon))
    return true;
  if (_isEq(v, 0, epsilon) && _isEq(end, 2*M_PI, epsilon))
    return true;

   return _validStartEnd0To2Pi (v, start, end);
}


#endif

