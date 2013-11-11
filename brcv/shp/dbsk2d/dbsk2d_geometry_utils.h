// This is brcv/shp/dbsk2d/dbsk2d_geometry_utils.h
#ifndef dbsk2d_geometry_utils_h_
#define dbsk2d_geometry_utils_h_
//:
// \file
// \brief This is 
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
// \endverbatim

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <dbsk2d/dbsk2d_utils.h>

// Vectors
//: dot product between two vectors
inline double _dot (double x1, double y1, double x2, double y2)
{
  return x1*x2+y1*y2;
}

//: dot product between two vectors
inline double _dot (VECTOR_TYPE v1, VECTOR_TYPE v2)
{
   return vcl_cos(v1)*vcl_cos(v2) + vcl_sin(v1)*vcl_sin(v2);
}

//: compute the vector between two points
inline VECTOR_TYPE _vPointPoint (vgl_point_2d<double> start, vgl_point_2d<double> end)
{
    return angle0To2Pi (vcl_atan2 ( end.y() - start.y(),
                     end.x() - start.x()) );
}

//: rotate a vector by the specified angle
inline vgl_vector_2d<double> _rotateCCW (double vx, double vy, double angle)
{
  vgl_vector_2d<double> vec;
  vec.set (vx*vcl_cos(angle) - vy*vcl_sin(angle),
           vx*vcl_sin(angle) + vy*vcl_cos(angle));
  return vec;
}

//; rotate a vector by the specified angle
inline vgl_vector_2d<double> _rotateCCW (vgl_vector_2d<double> v, double angle)
{
  vgl_vector_2d<double> vec;
  vec.set( v.x()*vcl_cos(angle) - v.y()*vcl_sin(angle),
           v.x()*vcl_sin(angle) + v.y()*vcl_cos(angle));
  return vec;
}

//Geometry: Points

//: compute the new position of a point given the displacement direction and length
inline vgl_point_2d<double> _translatePoint (vgl_point_2d<double> pt, VECTOR_TYPE v, double length)
{
  vgl_point_2d<double> dpt;
  dpt.x() = pt.x() + length*vcl_cos(v);
  dpt.y() = pt.y() + length*vcl_sin(v);
  return dpt;
}

//: compute the new position of a point given the displacement vector
inline vgl_point_2d<double> _translatePoint (vgl_point_2d<double> pt, vgl_vector_2d<double>vec)
{
  return pt+vec;
}

//: distance between two points
inline double _distPointPoint (vgl_point_2d<double> p1, vgl_point_2d<double> p2)
{
  return vnl_math_hypot((p2.x()-p1.x()), (p2.y()-p1.y())); 
}

//: square of the distance between two points
inline double _distSqPointPoint (vgl_point_2d<double> p1, vgl_point_2d<double> p2)
{
  return (p2.x()-p1.x())*(p2.x()-p1.x())+
       (p2.y()-p1.y())*(p2.y()-p1.y());
}

//: midpoint of two points
inline vgl_point_2d<double> _midPointPoint (vgl_point_2d<double> p1, vgl_point_2d<double> p2)
{
   vgl_point_2d<double> pt;
   pt.x() = (p1.x()+p2.x())*0.5;
   pt.y() = (p1.y()+p2.y())*0.5;
   return pt;
}

//: are two points approximately in the same place?
inline bool _BisEqPoint(vgl_point_2d<double> pt1, vgl_point_2d<double> pt2)
{
   return (BisEq(pt1.x(), pt2.x()) && BisEq(pt1.y(), pt2.y()));
}

//Geometry: Lines

//: determines if three points are ordered CW or CCW
//return +1 if p0, p1, p2 is CCW
//return -1 if p0, p1, p2 is CW
//return  0 if p0, p1, p2 is a straight line
inline int _CCW (vgl_point_2d<double>& p0, vgl_point_2d<double>& p1, vgl_point_2d<double>& p2)
{
  float p0x = (float)p0.x();
  float p0y = (float)p0.y();
  float p1x = (float)p1.x();
  float p1y = (float)p1.y();
  float p2x = (float)p2.x();
  float p2y = (float)p2.y();

  float dx1 = p1x - p0x;
  float dy1 = p1y - p0y;
  float dx2 = p2x - p0x;
  float dy2 = p2y - p0y;

  if (dx1*dy2 > dy1*dx2)
    return +1;
  if (dx1*dy2 < dy1*dx2)
    return -1;

  if ((dx1*dx2 < 0) || (dy1*dy2 < 0))
    return -1;

  if ((dx1*dx1+dy1*dy1) < (dx2*dx2+dy2*dy2))
    return +1;

  return 0;
}

//: determines if the lines segments specified by two pairs of points intersect
inline bool _areLinesIntersecting (vgl_point_2d<double>& line1s, vgl_point_2d<double>& line1e, 
                                   vgl_point_2d<double>& line2s, vgl_point_2d<double>& line2e)
{
  return ( (_CCW(line1s, line1e, line2s)*_CCW(line1s, line1e, line2e)) <=0 ) &&
       ( (_CCW(line2s, line2e, line1s)*_CCW(line2s, line2e, line1e)) <=0 );
}

inline double _getT (vgl_point_2d<double> pt, vgl_point_2d<double> lstart, 
                     vgl_point_2d<double> lend)
{
  //see matlab/pointline.m
  //Ken: divide is slow.
  return ((pt.y()-lstart.y())*(lend.y()-lstart.y()) + 
          (pt.x()-lstart.x())*(lend.x()-lstart.x()) ) /
         ((lend.y()-lstart.y())*(lend.y()-lstart.y()) + 
          (lend.x()-lstart.x())*(lend.x()-lstart.x()) );
}

inline double _distPointLine (vgl_point_2d<double> pt, vgl_point_2d<double> lstart, 
                              vgl_point_2d<double> lend)
{
  //Ken: divide is slow.
  return vcl_fabs((pt.y()-lstart.y())*(lend.x()-lstart.x()) - 
                  (pt.x()-lstart.x())*(lend.y()-lstart.y()))/ 
         vcl_sqrt((lend.x()-lstart.x())*(lend.x()-lstart.x()) + 
                  (lend.y()-lstart.y())*(lend.y()-lstart.y()));
}

inline double _distSqPointLine (vgl_point_2d<double> pt, vgl_point_2d<double> lstart, 
                                vgl_point_2d<double> lend)
{
  //Ken: divide is slow.
  double nom = (pt.y()-lstart.y())*(lend.x()-lstart.x()) - 
               (pt.x()-lstart.x())*(lend.y()-lstart.y());
  return nom*nom / ((lend.x()-lstart.x())*(lend.x()-lstart.x()) + 
                    (lend.y()-lstart.y())*(lend.y()-lstart.y()));
}

inline double _distPointLineSegment (vgl_point_2d<double> pt, 
                                     vgl_point_2d<double> lstart, 
                                     vgl_point_2d<double> lend)
{
  //Ken: divide is slow.
  double l = vcl_sqrt((lend.y()-lstart.y())*(lend.y()-lstart.y()) + 
                      (lend.x()-lstart.x())*(lend.x()-lstart.x()));

  double t = ((pt.y()-lstart.y())*(lend.y()-lstart.y()) + 
              (pt.x()-lstart.x())*(lend.x()-lstart.x()))/l;

  if (t<0)
    return vcl_sqrt((pt.y()-lstart.y())*(pt.y()-lstart.y()) +
                    (pt.x()-lstart.x())*(pt.x()-lstart.x()));
  else if (t>l)
    return vcl_sqrt((pt.y()-lend.y())*(pt.y()-lend.y()) +
                    (pt.x()-lend.x())*(pt.x()-lend.x()));
  else
    return vcl_fabs((pt.y()-lstart.y())*(lend.x()-lstart.x()) - 
                  (pt.x()-lstart.x())*(lend.y()-lstart.y()))/l;
}

inline vgl_point_2d<double> _getFootPt (vgl_point_2d<double> pt, vgl_point_2d<double> lstart, 
                                        vgl_point_2d<double> lend)
{
  double t = _getT (pt, lstart, lend);
  return lstart + t*(lend-lstart);
}

//TO SPEED-UP: AVOID UNNECESSARY ASSIGNMENTS.
inline vgl_point_2d<double> _getFootPt (vgl_point_2d<double> lstart, 
                                        vgl_point_2d<double> lend, double t)
{
  return lstart + t*(lend-lstart);
}

//note that delta is a 'signed' distance
inline double _deltaPointLine (vgl_point_2d<double> pt, 
    vgl_point_2d<double> lstart, vgl_point_2d<double> lend)
{
  //Ken: divide is slow.
  return ((pt.y()-lstart.y())*(lend.y()-lstart.y()) + 
          (pt.x()-lstart.x())*(lend.x()-lstart.x())) /
 vcl_sqrt((lend.y()-lstart.y())*(lend.y()-lstart.y()) + 
          (lend.x()-lstart.x())*(lend.x()-lstart.x()));
}

//note that delta is a 'signed' distance
inline double _deltaPointLine (vgl_point_2d<double> pt, 
    vgl_point_2d<double> lstart, vgl_point_2d<double> lend, double line_length)
{
  //Ken: divide is slow.
  return ((pt.y()-lstart.y())*(lend.y()-lstart.y()) + 
          (pt.x()-lstart.x())*(lend.x()-lstart.x())) / line_length;
}

inline bool _isPointAboveLine (vgl_point_2d<double> pt, 
                               vgl_point_2d<double> lstart, vgl_point_2d<double> lend)
{
  return ((pt.x()-lstart.x())*(lstart.y()-lend.y())+(pt.y()-lstart.y())*(lend.x()-lstart.x()))>0;
}

inline VECTOR_TYPE _vPointLine (vgl_point_2d<double> pt, 
                                vgl_point_2d<double> lstart, vgl_point_2d<double> lend)
{
  //making this computation consistent with later computations
  VECTOR_TYPE vSP = _vPointPoint (lstart, pt);
  VECTOR_TYPE _u =_vPointPoint (lstart, lend);
  VECTOR_TYPE _n = angle0To2Pi (_u + vnl_math::pi_over_2);
  
  if (_dot (_n, vSP)>0)
    return angle0To2Pi (_u - vnl_math::pi_over_2);
  else
    return _n;
}

//Geometry: Rectangles

inline bool _isPointInsideRect (vgl_point_2d<double> pt, double l, double t, double r, double b)
{
  //Ken:
  //if both a & b are positive and float (not double)
   //if (a < b)
  //if (*(long *)&a < *(long *)&b)
  
  if (pt.x()>= l && pt.x()<=r && pt.y()>= t && pt.y()<=b)
    return true;
  else
    return false;
}

inline bool _areTwoRectsIntersecting (double L1, double T1, double R1, double B1,
                         double L2, double T2, double R2, double B2)
{
  if (L2<R1 && R2>L1 && T2<B1 && B2>T1)
    return true;
  else
    return false;
}

//Geometry: Arcs

enum ARC_NUD{
  BOGUS_ARC_NUD=0,
  ARC_NUD_CCW=-1,
  ARC_NUD_CW=+1
};

enum ARC_NUS {
  BOGUS_ARC_NUS=0,
  ARC_NUS_LARGE=-1,
  ARC_NUS_SMALL=+1
};

//: get the center point of an arc
inline vgl_point_2d<double> getCenterOfArc (double sx, double sy, double ex, double ey, 
               double r, ARC_NUD nud, ARC_NUS nus)
{
  int nu=0; //not possible just a precaution
  double tau;
  double d = vcl_sqrt((ey-sy)*(ey-sy) + (ex-sx)*(ex-sx));
  
  if (_isEq(d,2*r, R_EPSILON)) tau=0;
  else              tau = vcl_acos(d/(2*r)); 

  double psi = vcl_atan2(ey-sy, ex-sx) + vnl_math::pi_over_2; 
  psi = angle0To2Pi (psi);

  if (nud>0 && nus>0) nu=-1;
  else if (nud>0 && nus<0) nu=+1;
  else if (nud<0 && nus>0) nu=+1;
  else if (nud<0 && nus<0) nu=-1;
  else dbsk2d_assert(false); 

  return vgl_point_2d<double> ((sx+ex)/2 + (d/2)*vcl_tan(nu*tau)*vcl_cos(psi),
                               (sy+ey)/2 + (d/2)*vcl_tan(nu*tau)*vcl_sin(psi));
}

//: get the center of the circular arc fitting to the three points
// (x1,y1)-----(x3,y3)-----(x2,y2)
inline vgl_point_2d<double> getArcCenterFromThreePoints (double x1, double y1, 
                        double x2, double y2,
                        double x3, double y3)
{
  vgl_point_2d<double> center;

  double start1x = (x1+x3)/2;
  double start1y = (y1+y3)/2;

  double start2x = (x2+x3)/2;
  double start2y = (y2+y3)/2;

  double psi1 = vcl_atan2(y3-y1,x3-x1) + vnl_math::pi_over_2;
  double psi2 = vcl_atan2(y3-y2,x3-x2) + vnl_math::pi_over_2;

  double psihat = vcl_atan2(start2y - start1y, start2x - start1x);

  if (vcl_sin(psi2 - psi1)==0){// parallel lines
    center.set(100000, 100000);
  }
  else {
    double test1 = vcl_sin(psi2 - psihat )/vcl_sin(psi2 - psi1);
    double newH = vcl_sqrt( (start1y - start2y)*(start1y - start2y) +
                  (start1x - start2x)*(start1x - start2x) );

    center.set( start1x + newH*test1*vcl_cos(psi1),
                start1y + newH*test1*vcl_sin(psi1));
  }
  return center;
}

inline double getArcRadiusFromThreePoints(double x1, double y1, 
                        double x2, double y2,
                        double x3, double y3)
{
  vgl_point_2d<double> center;

  double start1x = (x1+x3)/2;
  double start1y = (y1+y3)/2;

  double start2x = (x2+x3)/2;
  double start2y = (y2+y3)/2;

  double psi1 = vcl_atan2(y3-y1,x3-x1) + vnl_math::pi_over_2;
  double psi2 = vcl_atan2(y3-y2,x3-x2) + vnl_math::pi_over_2;

  double psihat = vcl_atan2(start2y - start1y, start2x - start1x);

  if (vcl_sin(psi2 - psi1)==0){// parallel lines
    return ISHOCK_DIST_HUGE;
  }
  else {
    double test1 = vcl_sin(psi2 - psihat )/vcl_sin(psi2 - psi1);
    double newH = vcl_sqrt( (start1y - start2y)*(start1y - start2y) +
                  (start1x - start2x)*(start1x - start2x) );

    center.set( start1x + newH*test1*vcl_cos(psi1),
                start1y + newH*test1*vcl_sin(psi1));

    return _distPointPoint(center, vgl_point_2d<double>(x1,y1));
  }
}

//: get the radius of the circular arc fitting to the three points
// Pt1-----Pt2----Pt3
inline double getArcRadiusFromThreePoints (vgl_point_2d<double> Pt1, vgl_point_2d<double> Pt2, vgl_point_2d<double> Pt3)
{
  return getArcRadiusFromThreePoints(Pt1.x(), Pt1.y(), Pt3.x(), Pt3.y(), Pt2.x(), Pt2.y());
}

//: get the tangent of the point (x3,y3) from the circular estimation of 
//three points: (x1,y1)-----(x3,y3)-----(x2,y2)
//return tangent in [0 ~ 2Pi)
//choose the tangent with the same direction of the chord (x1,y1)->(x2,y2)
//use dot product to exam it.
inline double getTangentFromThreePoints (double x1, double y1, 
                       double x2, double y2,
                       double x3, double y3)
{
  double vcl_tan_chord = angle0To2Pi (vcl_atan2 (y2-y1, x2-x1));
  vgl_point_2d<double> center = getArcCenterFromThreePoints (x1, y1, x2, y2, x3, y3);
  
  if (center.x()==100000)
    return vcl_tan_chord;

  double tangent1 = angle0To2Pi (vcl_atan2 (center.y()-y3, center.x()-x3) + vnl_math::pi_over_2);
  double tangent2 = angle0To2Pi (vcl_atan2 (center.y()-y3, center.x()-x3) - vnl_math::pi_over_2);
  double tangent;
  if (_dot (vcl_tan_chord, tangent1)>0) 
    tangent = tangent1;
  else
    tangent = tangent2;
  return tangent;
}

//: another version of get center of an arc
inline vgl_point_2d<double> centerOfArc(const vgl_point_2d<double> &point1, 
    const vgl_point_2d<double> &point2, double &radius, ARC_NUD &nud, ARC_NUS &nus)
{
  double chordVector = vcl_atan2(point2.y()- point1.y(), point2.x()-point1.x());
  vgl_point_2d<double> midChord((point2.x()+point1.x())/2, (point2.y()+point1.y())/2);

  double chordL = vnl_math_hypot(point2.y()- point1.y(), point2.x()-point1.x());
  double d = vcl_sqrt(radius*radius - chordL*chordL/4);

  double direction = chordVector;
  if (nud == ARC_NUD_CCW)
    if (nus == ARC_NUS_SMALL) direction += vnl_math::pi/2;
    else direction -= vnl_math::pi/2;
  else
    if (nus == ARC_NUS_SMALL) direction -= vnl_math::pi/2;
    else direction += vnl_math::pi/2;

  vgl_point_2d<double> center = _translatePoint(midChord, direction , d);
  return center;
}

// given three points, p1, p2 , and p3 - find the center and radius
// of the circle that passes thru them
inline bool threePointsToArc (const vgl_point_2d<double> &p1, const vgl_point_2d<double> &p2, 
                    const vgl_point_2d<double> &p3, vgl_point_2d<double> &center, double& radius, ARC_NUD &nud)
{
   double A = p2.x() - p1.x();
   double B = p2.y() - p1.y();
   double C = p3.x() - p1.x();
   double D = p3.y() - p1.y();
   
   double E = A*(p1.x() + p2.x()) + B*(p1.y() + p2.y());
   double F = C*(p1.x() + p3.x()) + D*(p1.y() + p3.y());
   
   double G = 2.0*(A*(p3.y() - p2.y())-B*(p3.x() - p2.x()));

   if (G == 0) return false;   // points are collinear
   
   center.set ( (D*E - B*F)/G, (A*F - C*E)/G);

   radius = vcl_sqrt( (center.x()-p1.x())*(center.x()-p1.x()) +
                  (center.y()-p1.y())*(center.y()-p1.y()) );

  if (A*D-B*C > 0)
    nud = ARC_NUD_CCW;
  else
    nud = ARC_NUD_CW;

   return true;
}

/*inline bool threePointsToArc(const vgl_point_2d<double> &point1,
    const vgl_point_2d<double> &point2, const vgl_point_2d<double> &point3,
    vgl_point_2d<double> &center, double &radius, ARC_NUD &nud) 
{
  double t, H;

  double 
    s1x = (point1.x()+point2.x())/2, 
    s1y = (point1.y()+point2.y())/2,
    s2x = (point2.x()+point3.x())/2,
    s2y = (point2.y()+point3.y())/2;

  double 
    psi1 = vcl_atan2(point2.y()-point1.y(),point2.x()-point1.x()) + vnl_math::pi/2,
    psi2 = vcl_atan2(point2.y()-point3.y(),point2.x()-point3.x()) + vnl_math::pi/2;

  double psihat = vcl_atan2(s2y-s1y, s2x-s1x);

  if (vcl_sin(psi2 - psi1)==0) // collinear
    return false;
  else {
    t = vcl_sin(psi2 - psihat )/vcl_sin(psi2 - psi1);
    H = vnl_math_hypot(s1y-s2y, s1x-s2x);

    center.setX(s1x + H*t*vcl_cos(psi1));
    center.setY(s1y + H*t*vcl_sin(psi1));

    radius = vnl_math_hypot(center.x()-point1.x(), 
              center.y()-point1.y());
  }

  if (t<0) nud = ARC_NUD_CW;
  else    nud = ARC_NUD_CCW;

  return true;
}*/

inline bool threePointsToArc(const vgl_point_2d<double> &point1,
    const vgl_point_2d<double> &point2, const vgl_point_2d<double> &point3,
    vgl_point_2d<double> &center, double &radius, double &theta_first, double &theta_second) 
{
  double
    x1 = point1.x(), x2 = point2.x(), x3 = point3.x(),
    y1 = point1.y(), y2 = point2.y(), y3 = point3.y(),
    l1 = x1*x1+y1*y1,   l2 = x2*x2+y2*y2,   l3 = x3*x3+y3*y3;

  // from http://mathworld.wolfram.com/Circle.html
  double a, d, e, f;
  a = (x2*y3-x3*y2) - (x1*y3-x3*y1) + (x1*y2-x2*y1);
  d = -((l2*y3 - y2*l3) - (l1*y3-l3*y1) + (l1*y2-y1*l2));
  e = (l2*x3 - x2*l3) - (l1*x3-l3*x1) + (l1*x2-x1*l2);
  f = -(y1*(l2*x3 - x2*l3) - y2*(l1*x3-l3*x1) + y3*(l1*x2-x1*l2));

  if(vcl_fabs(a) < 1e-15)
   return false; // in a straight line

  double r = vcl_sqrt((d*d+e*e)/(4*a*a) - f/a);
  double cx = -d/(2*a), cy = -e/(2*a);

  double theta1 = vcl_atan2(y1-cy, x1-cx),
  theta2 = vcl_atan2(y2-cy, x2-cx),
  theta3 = vcl_atan2(y3-cy, x3-cx);

  // make sure they're in ascending order
  if(theta3 < theta1) theta3 += 2*vnl_math::pi;
  if(theta2 < theta1) theta2 += 2*vnl_math::pi;

  double thetamin = (theta1 < theta3) ? theta1 : theta3;
  double thetamax = (theta1 < theta3) ? theta3 : theta1;

  if(!(thetamin < theta2 && thetamax > theta2)) {
    // theta 2 doesn't lie between theta1 and theta3, so go the other way around the circle
    std::swap(theta1, theta3);
  }

  center.set(cx,cy);
  radius = r;
  theta_first = theta1;
  theta_second = theta3;
  return true;
}

inline void pointTangentPointToArc(const vgl_point_2d<double> &point1, double theta, 
    const vgl_point_2d<double> &point2, vgl_point_2d<double> &center, 
    double &radius, ARC_NUD &nud) 
{
  double x1 = point1.x(), y1 = point1.y(),
  x2 = point2.x(), y2 = point2.y();
  double u = (x2-x1)*vcl_cos(theta) + (y2-y1)*vcl_sin(theta),
  v = -(x2-x1)*vcl_sin(theta) + (y2-y1)*vcl_cos(theta);

  double r = .5*(v*v+u*u)/v;
  double cx = x1 - r*vcl_sin(theta), cy = y1 + r*vcl_cos(theta);

  if(r < 0) {
    r = -r;
    nud = ARC_NUD_CCW;
  }
  else
    nud = ARC_NUD_CW;

  center.set(cx, cy);
  radius = r;
}

inline void pointTangentPointToArc(const vgl_point_2d<double> &point1, double theta, const vgl_point_2d<double> &point2,
    vgl_point_2d<double> &center, double &radius, double &theta_first, double &theta_second) 
{
  double x1 = point1.x(), y1 = point1.y(),
  x2 = point2.x(), y2 = point2.y();
  double u = (x2-x1)*vcl_cos(theta) + (y2-y1)*vcl_sin(theta),
  v = -(x2-x1)*vcl_sin(theta) + (y2-y1)*vcl_cos(theta);

  double r = .5*(v*v+u*u)/v;
  double cx = x1 - r*vcl_sin(theta), cy = y1 + r*vcl_cos(theta);
  double theta1 = vcl_atan2(y1-cy, x1-cx),
  theta2 = vcl_atan2(y2-cy, x2-cx);

  if(r < 0) {
    r = -r;
    std::swap(theta1, theta2);
  }

  center.set(cx, cy);
  radius = r;
  theta_first = theta1;
  theta_second = theta2;
}

inline double getTangentOfArc(const vgl_point_2d<double> &center,
    const vgl_point_2d<double> &start, const vgl_point_2d<double> &end, ARC_NUD nud) 
{
  double thetai = vcl_atan2(end.y()-center.y(), end.x()-center.x());

  if (nud==ARC_NUD_CCW)
    thetai -= vnl_math::pi/2;
  else
    thetai += vnl_math::pi/2;

  return thetai;
}

inline double getTangentOfArc(const vgl_point_2d<double> &center, double radius, double theta_first, double theta_second,
    const vgl_point_2d<double> &start, const vgl_point_2d<double> &point) 
{
  vgl_vector_2d<double> subtangenti = point-center;
  double thetai = vcl_atan2(subtangenti.y(), subtangenti.x());

  vgl_vector_2d<double> start_subtan = start - center;
  double theta_start = vcl_atan2(start_subtan.y(), start_subtan.x());
  if(vcl_fabs( vcl_fmod (theta_start - theta_first, 2*vnl_math::pi)) < 1e-4)
    thetai += vnl_math::pi/2;
  else
    thetai -= vnl_math::pi/2;
  return thetai;
}

// commented becaus similar function already exists in vgl
////: Return a point on a line segment, specified by the ratio between its length to the starting point
//// and the length of the line segment.
//// Note if ratio < 0 or ratio > 1 then the point lies outside the line segment
//inline vgl_point_2d<double > point_on_line_segment(double sx, double sy, double ex, double ey, double ratio)
//{ 
//  return vgl_point_2d<double >(ex*ratio + sx*(1-ratio), ey*ratio + sy*(1-ratio));
//};
//
//inline vgl_point_2d<double > point_on_line_segment(const vgl_line_segment_2d<double >& line, double ratio)
//{ 
//  return point_on_line_segment(line.point1().x(), line.point1().y(), line.point2().x(), line.point2().y(), ratio);
//};


#endif // dbsk2d_geometry_utils_h_

