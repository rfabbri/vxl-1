//ISHOCK-COMMON.CPP
#include <extrautils/msgout.h>
#include "ishock-common.h"
#include "ishock.h"

////////////////////////////////////////////////
// Common struct definition...
Point INFINITY_POINT (INFINITY_POINT_X, INFINITY_POINT_Y);
Point INVALID_POINT  (INVALID_POINT_X,  INVALID_POINT_Y);


//EPSILON ISSUE: here we use AisEq, so _validStartEnd0To2Pi is fuzzy, too.
//NEED TO BE RE-WRITE!
//EndPoint Included
bool _validStartEnd0To2PiEPIncld (VECTOR_TYPE v, VECTOR_TYPE start, VECTOR_TYPE end)
{
   if (AisEq(v,start) || AisEq(v,end))
    return true;

  if (AisEq(v,2*M_PI) && AisEq(start,0))
    return true;
  if (AisEq(v,0) && AisEq(start,2*M_PI))
    return true;
  if (AisEq(v,2*M_PI) && AisEq(end,0))
    return true;
  if (AisEq(v,0) && AisEq(end,2*M_PI))
    return true;

   return _validStartEnd0To2Pi (v, start, end);
}

///////////////////////////////////////////////////////
// BASIC GROMETRY: POINT-LINE

Point _getValidFootPt (Point pt, Point lstart, Point lend)
{
   double x1 = lstart.x;
   double y1 = lstart.y;
   double x2 = lend.x;
   double y2 = lend.y;

  double t = _getT (pt, lstart, lend);
  assert (t>=0 && t<=1);

  Point foot;
   foot.x = x1 + t* (x2-x1);
   foot.y = y1 + t* (y2-y1);

   return foot;
}

Point _getValidFootPt (Point pt, Point lstart, Point lend, double t)
{
   double x1 = lstart.x;
   double y1 = lstart.y;
   double x2 = lend.x;
   double y2 = lend.y;

  assert (t>=0 && t<=1);

  Point foot;
   foot.x = x1 + t* (x2-x1);
   foot.y = y1 + t* (y2-y1);

   return foot;
}

VECTOR_TYPE _vPointLine (Point pt, Point lstart, Point lend)
{
   Point foot = _getFootPt (pt, lstart, lend);
   return _vPointPoint (pt, foot);
}

//note that delta is a 'signed' distance
DIST_TYPE _deltaPointLine (Point pt, Point lstart, Point lend)
{
  double t = _getT (pt, lstart, lend);

  Point foot = _getFootPt (lstart, lend, t);

   DIST_TYPE delta = _distPointPoint (lstart, foot);
   if (t>0)
      return delta;
   else
      return -delta;
}

///////////////////////////////////////////////////////
// BASIC GROMETRY: POINT-ARC
Point getCenterOfArc (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey, 
               DIST_TYPE r, ARC_NUD nud, ARC_NUS nus)
{
   Point cen;
   int nu;

   DIST_TYPE d = vcl_sqrt((ey-sy)*(ey-sy) + (ex-sx)*(ex-sx));
  double tau;
  if (AisEq(d,2*r)) tau=0;
   else              tau = vcl_acos(d/(2*r)); 
   double psi = vcl_atan2(ey-sy, ex-sx) + M_PI_2; 
   psi = angle0To2Pi (psi);

   if (nud>0 && nus>0) nu=-1;
   else if (nud>0 && nus<0) nu=+1;
   else if (nud<0 && nus>0) nu=+1;
   else if (nud<0 && nus<0) nu=-1;   

   cen.x = (sx+ex)/2 + (d/2)*vcl_tan(nu*tau)*vcl_cos(psi);
   cen.y = (sy+ey)/2 + (d/2)*vcl_tan(nu*tau)*vcl_sin(psi);

   return cen;
}

//get the center of the circular arc fitting to the three points
// (x1,y1)-----(x3,y3)-----(x2,y2)
Point getArcCenterFromThreePoints (COORD_TYPE x1, COORD_TYPE y1, 
                        COORD_TYPE x2, COORD_TYPE y2,
                        COORD_TYPE x3, COORD_TYPE y3)
{
  Point center;

  double start1x = (x1+x3)/2;
  double start1y = (y1+y3)/2;

  double start2x = (x2+x3)/2;
  double start2y = (y2+y3)/2;

  double psi1 = vcl_atan2(y3-y1,x3-x1) + M_PI_2;
  double psi2 = vcl_atan2(y3-y2,x3-x2) + M_PI_2;

  double psihat = vcl_atan2(start2y - start1y, start2x - start1x);

  if (vcl_sin(psi2 - psi1)==0){// parallel lines
    center.x=100000;
    center.y=100000;
  }
  else {
    double test1 = vcl_sin(psi2 - psihat )/vcl_sin(psi2 - psi1);
    double newH = vcl_sqrt( (start1y - start2y)*(start1y - start2y) +
                  (start1x - start2x)*(start1x - start2x) );

    center.x = start1x + newH*test1*vcl_cos(psi1);
    center.y = start1y + newH*test1*vcl_sin(psi1);
  }

  return center;
}

double getArcRadiusFromThreePoints(COORD_TYPE x1, COORD_TYPE y1, 
                        COORD_TYPE x2, COORD_TYPE y2,
                        COORD_TYPE x3, COORD_TYPE y3)
{
  Point center;

  double start1x = (x1+x3)/2;
  double start1y = (y1+y3)/2;

  double start2x = (x2+x3)/2;
  double start2y = (y2+y3)/2;

  double psi1 = vcl_atan2(y3-y1,x3-x1) + M_PI_2;
  double psi2 = vcl_atan2(y3-y2,x3-x2) + M_PI_2;

  double psihat = vcl_atan2(start2y - start1y, start2x - start1x);

  if (vcl_sin(psi2 - psi1)==0){// parallel lines
    return ISHOCK_DIST_HUGE;
  }
  else {
    double test1 = vcl_sin(psi2 - psihat )/vcl_sin(psi2 - psi1);
    double newH = vcl_sqrt( (start1y - start2y)*(start1y - start2y) +
                  (start1x - start2x)*(start1x - start2x) );

    center.x = start1x + newH*test1*vcl_cos(psi1);
    center.y = start1y + newH*test1*vcl_sin(psi1);

    return _distPointPoint(center, Point(x1,y1));
  }
}

//get the radius of the circular arc fitting to the three points
// Pt1-----Pt2----Pt3
double getArcRadiusFromThreePoints (Point Pt1, Point Pt2, Point Pt3)
{
  return getArcRadiusFromThreePoints(Pt1.x, Pt1.y, Pt3.x, Pt3.y, Pt2.x, Pt2.y);
}

//get the tangent of the point (x3,y3) from the circular estimation of 
//three points: (x1,y1)-----(x3,y3)-----(x2,y2)
//return tangent in [0 ~ 2Pi)
//choose the tangent with the same direction of the chord (x1,y1)->(x2,y2)
//use dot product to exam it.
double getTangentFromThreePoints (COORD_TYPE x1, COORD_TYPE y1, 
                       COORD_TYPE x2, COORD_TYPE y2,
                       COORD_TYPE x3, COORD_TYPE y3)
{
  double vcl_tan_chord = angle0To2Pi (vcl_atan2 (y2-y1, x2-x1));
  Point center = getArcCenterFromThreePoints (x1, y1, x2, y2, x3, y3);
  if (center.x==100000)
    return vcl_tan_chord;

  double tangent1 = angle0To2Pi (vcl_atan2 (center.y-y3, center.x-x3) + M_PI_2);
  double tangent2 = angle0To2Pi (vcl_atan2 (center.y-y3, center.x-x3) - M_PI_2);
  double tangent;
  if (_angle_vector_dot (vcl_tan_chord, tangent1)>0) 
    tangent = tangent1;
  else
    tangent = tangent2;
  return tangent;
}

///////////////////////////////////////////////////////
// BASIC GEOMETRY:

//Are points equal
bool _BisEqPoint(Point pt1, Point pt2)
{
   return (BisEq(pt1.x, pt2.x) && BisEq(pt1.y, pt2.y));
}
