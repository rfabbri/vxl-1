#ifndef ARC_H_INCLUDED
#define ARC_H_INCLUDED

#include "points.h"
#include "geometry_functions.h"

typedef enum {
  BOGUS_ARC_NUD=0,
  ARC_NUD_CCW=-1,
  ARC_NUD_CW=+1
} ARC_NUD;

typedef enum {
  BOGUS_ARC_NUS=0,
  ARC_NUS_LARGE=-1,
  ARC_NUS_SMALL=+1
} ARC_NUS;

inline Point2D<double> centerOfArc(const Point2D<double> &point1, 
    const Point2D<double> &point2, double &radius, ARC_NUD &nud, ARC_NUS &nus)
{
  double chordVector = vcl_atan2(point2.getY()- point1.getY(), point2.getX()-point1.getX());
  Point2D<double> midChord = (point1+point2)/2;

  double chordL = hypot(point2.getY()- point1.getY(), point2.getX()-point1.getX());
  double d = vcl_sqrt(radius*radius - chordL*chordL/4);

  double direction = chordVector;
  if (nud == ARC_NUD_CCW)
    if (nus == ARC_NUS_SMALL) direction += M_PI/2;
    else direction -= M_PI/2;
  else
    if (nus == ARC_NUS_SMALL) direction -= M_PI/2;
    else direction += M_PI/2;

  Point2D<double> center = vectorPoint(midChord, direction , d);
  return center;
}

// given three points, p1, p2 , and p3 - find the center and radius
// of the circle that passes thru them
inline bool threePointsToArc (const Point2D<double> &p1, const Point2D<double> &p2, 
                    const Point2D<double> &p3, Point2D<double> &center, double& radius, ARC_NUD &nud)
{
   double A = p2.x() - p1.x();
   double B = p2.y() - p1.y();
   double C = p3.x() - p1.x();
   double D = p3.y() - p1.y();
   
   double E = A*(p1.x() + p2.x()) + B*(p1.y() + p2.y());
   double F = C*(p1.x() + p3.x()) + D*(p1.y() + p3.y());
   
   double G = 2.0*(A*(p3.y() - p2.y())-B*(p3.x() - p2.x()));

   if (G == 0) return false;   // points are collinear
   
   center.setX ((D*E - B*F) / G);
   center.setY ((A*F - C*E) / G);

   radius = vcl_sqrt( (center.x()-p1.x())*(center.x()-p1.x()) +
                  (center.y()-p1.y())*(center.y()-p1.y()) );

  if (A*D-B*C > 0)
    nud = ARC_NUD_CCW;
  else
    nud = ARC_NUD_CW;

   return true;
}

/*inline bool threePointsToArc(const Point2D<double> &point1,
    const Point2D<double> &point2, const Point2D<double> &point3,
    Point2D<double> &center, double &radius, ARC_NUD &nud) 
{
  double t, H;

  double 
    s1x = (point1.getX()+point2.getX())/2, 
    s1y = (point1.getY()+point2.getY())/2,
    s2x = (point2.getX()+point3.getX())/2,
    s2y = (point2.getY()+point3.getY())/2;

  double 
    psi1 = vcl_atan2(point2.getY()-point1.getY(),point2.getX()-point1.getX()) + M_PI/2,
    psi2 = vcl_atan2(point2.getY()-point3.getY(),point2.getX()-point3.getX()) + M_PI/2;

  double psihat = vcl_atan2(s2y-s1y, s2x-s1x);

  if (vcl_sin(psi2 - psi1)==0) // collinear
    return false;
  else {
    t = vcl_sin(psi2 - psihat )/vcl_sin(psi2 - psi1);
    H = hypot(s1y-s2y, s1x-s2x);

    center.setX(s1x + H*t*vcl_cos(psi1));
    center.setY(s1y + H*t*vcl_sin(psi1));

    radius = hypot(center.getX()-point1.getX(), 
              center.getY()-point1.getY());
  }

  if (t<0) nud = ARC_NUD_CW;
  else    nud = ARC_NUD_CCW;

  return true;
}*/

inline bool threePointsToArc(const Point2D<double> &point1,
    const Point2D<double> &point2, const Point2D<double> &point3,
    Point2D<double> &center, double &radius, double &theta_first, double &theta_second) 
{
  double
    x1 = point1.getX(), x2 = point2.getX(), x3 = point3.getX(),
    y1 = point1.getY(), y2 = point2.getY(), y3 = point3.getY(),
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
  if(theta3 < theta1) theta3 += 2*M_PI;
  if(theta2 < theta1) theta2 += 2*M_PI;

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

inline void pointTangentPointToArc(const Point2D<double> &point1, double theta, 
    const Point2D<double> &point2, Point2D<double> &center, 
    double &radius, ARC_NUD &nud) 
{
  double x1 = point1.getX(), y1 = point1.getY(),
  x2 = point2.getX(), y2 = point2.getY();
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

inline void pointTangentPointToArc(const Point2D<double> &point1, double theta, const Point2D<double> &point2,
    Point2D<double> &center, double &radius, double &theta_first, double &theta_second) 
{
  double x1 = point1.getX(), y1 = point1.getY(),
  x2 = point2.getX(), y2 = point2.getY();
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

inline double getTangentOfArc(const Point2D<double> &center,
    const Point2D<double> &start, const Point2D<double> &end, ARC_NUD nud) 
{
  double thetai = vcl_atan2(end.getY()-center.getY(), end.getX()-center.getX());

  if (nud==ARC_NUD_CCW)
    thetai -= M_PI/2;
  else
    thetai += M_PI/2;

  return thetai;
}

inline double getTangentOfArc(const Point2D<double> &center, double radius, double theta_first, double theta_second,
    const Point2D<double> &start, const Point2D<double> &point) {
  Point2D<double> subtangenti = point-center;
  double thetai = vcl_atan2(subtangenti.getY(), subtangenti.getX());

  Point2D<double> start_subtan = start - center;
  double theta_start = vcl_atan2(start_subtan.getY(), start_subtan.getX());
  if(vcl_fabs( vcl_fmod (theta_start - theta_first, 2*M_PI)) < 1e-4)
    thetai += M_PI/2;
  else
    thetai -= M_PI/2;
  return thetai;
}


#endif
