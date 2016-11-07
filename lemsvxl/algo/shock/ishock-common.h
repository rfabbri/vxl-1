#ifndef _ISHOCK_COMMON_H
#define _ISHOCK_COMMON_H

#include <extrautils/common.h>
#include <extrautils/arc.h>
//#include <vcl_algorithm.h>

//##########################################################
// IMPORTANT SYSTEM NUMERICAL PARAMETERS
// EPSILON FOR FUZZY MEASURES
// See "float.h" for standard defined precision

//Note that the input is in float.
//But the footPt of Point-Line has to be in double.
//There is no advantage to keep Point structure in float.
//Instead, in fileio.cpp, we read the point data and
//truncate into float and convert again to double.

#define  INPUT_COORD_TYPE  float
#define  INPUT_TAN_TYPE    float
#define  COORD_TYPE      double
#define  TAU_TYPE        double
#define  LTAU_TYPE      double
#define  RADIUS_TYPE      double
#define  DIST_TYPE      double
#define  VECTOR_TYPE      double
#define  ANGLE_TYPE      double


#define  DOUBLE_PRECISION    1E-15      //Trustable Double Precision (1E-15)
#define  EI              1E-5      //Relative Grid/World of Ei (1E-5)
#define  DIST_BOUND        1E-12      //DOUBLE_PRECISION/EI/100
                            //Important!! ErrorBound of distance computation. (1E-10)
                            //We still have 10 digits for worst case distance computation.
#define  BND_WORLD_SIZE      2000      //Boundary World Size [0-BND_WORLD_SIZE) 1000
#define  MAX_RADIUS        5000.0    //IGNORE all shock propagations if over this range
#define  SHK_WORLD_SIZE      12000      //BND_WORLD_SIZE+2*MAX_RADIUS; 
                            //Shock World Size [-MAX_RADIUS ~ BND_WORLD_SIZE+MAX_RADIUS)

#define  CONTACT_EPSILON    1.1E-5    //EPSILON for formation of contact shock (1E-6)
                            //Should be the same as A_EPSILON
#define  EP_EPSILON        1E-10      //1E-13
#define  TO_EPSILON        1E-13      //EPSILON for formation of TO shock
#define  B_EPSILON        EI        //EPSILON for Boundary Input
#define  A_EPSILON        1E-6      //EPSILON for point Tau and angle (1E-10)
#define  L_EPSILON        1E-13      //L_EPSILON is only used in L-L-L intersection!
#define  TINY_EPSILON      1E-13      //The same value as L_EPSILON
#define  R_EPSILON        1E-5      //1E-5 1E-14 1E-8

#define  MAX_BIARC_RADIUS    MAX_RADIUS  //Max Biarc radius before a stright line is used to approximate it

#define  LARGE_DRAWLENGTH    100      //7 For Parabola and Hyperbola drawing
#define  POINT_TANGENT_DRAWLENGTH  0.3

#define W_THRESHOLD        0.01      //width threshold to determine if an arc is replacable by a line
 
//Infinity Point
#define  INFINITY_POINT_X  MAX_RADIUS+1
#define  INFINITY_POINT_Y  MAX_RADIUS+1
#define  INVALID_POINT_X    MAX_RADIUS+2
#define  INVALID_POINT_Y    MAX_RADIUS+2
#define  INVALID_COORD    MAX_RADIUS+3

//File reading parameters
#define  CON_FILE_SAMPLE_POINT_DISTANCE  0.5

inline bool isPointValidInWorld (COORD_TYPE x, COORD_TYPE y)
{
  if (x<0 || x>BND_WORLD_SIZE)
    return false;
  if (y<0 || y>BND_WORLD_SIZE)
    return false;

  return true;
}

//BOUNDARY COMPARISON EPSILON
inline bool BisEq (COORD_TYPE a, COORD_TYPE b) { return _isEq(a, b, B_EPSILON); }
inline bool BisLEq (COORD_TYPE a, COORD_TYPE b) { return _isLEq(a, b, B_EPSILON); }
inline bool BisGEq (COORD_TYPE a, COORD_TYPE b) { return _isGEq(a, b, B_EPSILON); }
inline bool BisL (COORD_TYPE a, COORD_TYPE b) { return _isL(a, b, B_EPSILON); }
inline bool BisG (COORD_TYPE a, COORD_TYPE b) { return _isG(a, b, B_EPSILON); }

//EPSILONISSUE 25: WRITE A GOOD ROUNDING FUNCTION!!!
inline void Bround (INPUT_COORD_TYPE& value) {
  ///_round (value, B_EPSILON);
}
inline void Bround (COORD_TYPE& value) {
  ///_round (value, B_EPSILON);
}

//Angle: [0, 2Pi)
inline VECTOR_TYPE angle02Pi (VECTOR_TYPE angle)
{
  VECTOR_TYPE a;

  if (angle>=M_PI*2)
      a = angle - M_PI*2;
   else if (angle < 0) {
      a = angle + M_PI*2;

    if (a==M_PI*2) //very rare case! if angle=-4.4408920985006e-016
      a = 0;
  }
  else
    a = angle;

  assert (a>=0 && a<M_PI*2);

  return a;
}

//Angle: Fuzzy [0, 2Pi)
inline VECTOR_TYPE angle02PiFuzzy (VECTOR_TYPE angle)
{
  if (_isEq (angle, 0, A_EPSILON))
    return 0;

  if (_isEq (angle, M_PI*2, A_EPSILON))
    return 0;

  //if (_isEq (angle, -M_PI*2, A_EPSILON)) {
  //  assert (0);
  //  return 0;
  //}

  VECTOR_TYPE a;

  if (angle>M_PI*2)
      a = angle - M_PI*2;
   else if (angle < 0)
      a = angle + M_PI*2;
  else
    a = angle;

  assert (a>=0 && a<M_PI*2);

  return a;
}

//Tau: [0, 2Pi]
inline VECTOR_TYPE tau02Pi (VECTOR_TYPE tau)
{
  VECTOR_TYPE t;

  if (tau>M_PI*2)
      t = tau - M_PI*2;
   else if (tau < 0)
      t = tau + M_PI*2;
  else
    t = tau;

  assert (t>=0 && t<=M_PI*2);

  return t;
}

//Tau: Fuzzy [0, 2Pi]
inline VECTOR_TYPE tau02PiFuzzy (VECTOR_TYPE tau)
{
  if (_isEq (tau, 0, A_EPSILON))
    return 0;

  if (_isEq (tau, M_PI*2, A_EPSILON))
    return M_PI*2;

  //if (_isEq (angle, -M_PI*2, A_EPSILON)) {
  //  assert (0);
  //  return 0;
  //}

  VECTOR_TYPE t;

  if (tau>M_PI*2)
      t = tau - M_PI*2;
   else if (tau < 0)
      t = tau + M_PI*2;
  else
    t = tau;

  assert (t>=0 && t<=M_PI*2);

  return t;
}

//POINT&ARC TAU & ANGLE EPSILON
inline bool AisEq (VECTOR_TYPE a, VECTOR_TYPE b) {
  return _isEq(a, b, A_EPSILON);
}

//Basically the same as AisEq, deal with 0<->2Pi continuity issue.
inline bool AisEq02Pi (VECTOR_TYPE a, VECTOR_TYPE b) {
  //EPSILONISSUE 22: HANGLE the 0, 2*M_PI DISCONTINUTY
  if (a>M_PI_2*3)
    a-=2*M_PI;
  if (b>M_PI_2*3)
    b-=2*M_PI;
  return _isEq(a, b, A_EPSILON);
}

inline bool AisLEq (VECTOR_TYPE a, VECTOR_TYPE b) { return _isLEq(a, b, A_EPSILON); }
inline bool AisGEq (VECTOR_TYPE a, VECTOR_TYPE b) { return _isGEq(a, b, A_EPSILON); }
inline bool AisL (VECTOR_TYPE a, VECTOR_TYPE b) { return _isL(a, b, A_EPSILON); }
inline bool AisG (VECTOR_TYPE a, VECTOR_TYPE b) { return _isG(a, b, A_EPSILON); }

//RADIUS COMPARISON EPSILON
inline bool RisEq (RADIUS_TYPE a, RADIUS_TYPE b) { return _isEq(a, b, R_EPSILON); }
inline bool RisLEq (RADIUS_TYPE a, RADIUS_TYPE b) { return _isLEq(a, b, R_EPSILON); }
inline bool RisGEq (RADIUS_TYPE a, RADIUS_TYPE b) { return _isGEq(a, b, R_EPSILON); }
inline bool RisL (RADIUS_TYPE a, RADIUS_TYPE b) { return _isL(a, b, R_EPSILON); }
inline bool RisG (RADIUS_TYPE a, RADIUS_TYPE b) { return _isG(a, b, R_EPSILON); }

//LINES LENGTH EPSILON
inline bool LisEq (double a, double b) { return _isEq(a, b, L_EPSILON); }
inline bool LisLEq (double a, double b) { return _isLEq(a, b, L_EPSILON); }
inline bool LisGEq (double a, double b) { return _isGEq(a, b, L_EPSILON); }
inline bool LisL (double a, double b) { return _isL(a, b, L_EPSILON); }
inline bool LisG (double a, double b) { return _isG(a, b, L_EPSILON); }

bool _validStartEnd0To2PiEPIncld (VECTOR_TYPE v, VECTOR_TYPE start, VECTOR_TYPE end);

//##########################################################

//for GetInfo in SIElement.CPP
#define IDifExists(x) ((x)==(0)?(0):(x->id()))
#define TypeifExists(x) ((x)==(0)?(0):(x->type()))

//Epsilon for general fuzzy measures
//#define G_EPSILON  1E-10 //1E-8
//#define isG(a,b)      ((a)>(b)+2*G_EPSILON?1:0)
//#define isL(a,b)      ((a)+2*G_EPSILON<(b)?1:0)
//#define isGEq(a,b)    ((a)+2*G_EPSILON<(b)?0:1)
//#define isLEq(a,b)    ((a)>(b)+2*G_EPSILON?0:1)
//#define isEq(a,b)     (vcl_fabs((a)-(b))<G_EPSILON?1:0)

////////////////////////////////////////////////
// Common struct definition...
class Point
{
public:
  COORD_TYPE x, y; //double
  Point () {x=0; y=0;}
  Point (COORD_TYPE newx, COORD_TYPE newy)
  {
    x=newx;
    y=newy;
  }
  bool operator==(const Point &point)
  {
    return (x==point.x && y==point.y);
  }
  Point operator+(const Point &point)
  {
    return Point(x+point.x, y+ point.y);  
  }
  Point operator*(double a)
  {
    return Point(a*x, a*y);  
  }
  bool operator < (const Point &point) const
  {
    if (x!=point.x)
      return x<point.x;
    else
      return (y<point.y);
  }
};

//quick lazy hack function
//this kind of change should be done on the whole code
inline Point2D<double> P2P2D(Point pt)
{
  return Point2D<double>(pt.x, pt.y);
}

//void swap (double& v1, double& v2);
//bool _isVectorInRange (double v, double start, double end);
//bool _isVectorInRange2 (double v, double start, double end);
//bool _isVectorInRangeEPIncld (double v, double start, double end);
//bool _isVectorInRange2EPIncld (double v, double start, double end);

////////////////////////////////////////////////
// Vector of (double x, double y)
class Vector
{
public:
  double _x, _y;

  Vector () {}
  Vector (double newx, double newy)
  {
    _x = newx;
    _y = newy;
  }
  Vector (Point start, Point end)
  {
    _x = end.x-start.x;
    _y = end.y-start.y;
  }
};

inline double _dot (Vector& v1, Vector& v2)
{
   return (v1._x*v2._x + v1._y*v2._y);
}

inline double VectorLenSq (Vector& v)
{
   return (v._x*v._x + v._y*v._y);
}

///////////////////////////////////////////////////////
// BASIC GROMETRY: POINT

//SLOW!!!!
//try if can pass a unit vector (x,y)
inline Point rotateCCW (Point pt, double angle)
{
   double rptx = ((double)pt.x)*vcl_cos(angle) - ((double)pt.y)*vcl_sin(angle);
   double rpty = ((double)pt.x)*vcl_sin(angle) + ((double)pt.y)*vcl_cos(angle);

   return Point ((float)rptx, (float)rpty);
}

inline Point rotateCCW (double x, double y, double angle)
{
   Point rpt;

   rpt.x = (float) (x*vcl_cos(angle) - y*vcl_sin(angle));
   rpt.y = (float) (x*vcl_sin(angle) + y*vcl_cos(angle));

   return rpt;
}

inline DIST_TYPE _distPointPoint (Point p1, Point p2)
{
  return hypot(((double)p2.x-(double)p1.x), ((double)p2.y-(double)p1.y)); 
}

inline double _distSqPointPoint (Point p1, Point p2)
{
  return ((double)p2.x-(double)p1.x)*((double)p2.x-(double)p1.x)+
       ((double)p2.y-(double)p1.y)*((double)p2.y-(double)p1.y);
}

//SLOW
///USE REAL VECTOR HERE.
//DON"T NEED THIS CONVERSION.
inline VECTOR_TYPE _vPointPoint (Point startpoint, Point endpoint)
{
    return angle02Pi (vcl_atan2 ( (double)endpoint.y - (double)startpoint.y,
                     (double)endpoint.x - (double)startpoint.x) );
}

//Ken:
//a: angle
//c: cosine
//s: sin
#ifdef _WIN32
static _inline void dcossin (double a, double *c, double *s)
{
  _asm
  {
    fld a
    fsincos
    mov eax, c
    fstp qword ptr [eax]
    mov eax, s
    fstp qword ptr [eax]
  }
}
#endif

inline Point _vectorPoint (Point pt, VECTOR_TYPE vector, DIST_TYPE length)
{
  Point dpt;

#ifdef _WIN32
  #ifdef _MSC_VER //If run on Windows in Visual C++, call the optimized one.
    double f, g;
    dcossin(vector,&f,&g);
    dpt.x = f*length+pt.x;
    dpt.y = g*length+pt.y;
  #else //Else, just do the job in normal way.
    dpt.x = pt.x + length*vcl_cos(vector);
    dpt.y = pt.y + length*vcl_sin(vector);
  #endif
#else
   dpt.x = pt.x + length*vcl_cos(vector);
   dpt.y = pt.y + length*vcl_sin(vector);
#endif

  return dpt;
}

inline Point _midPointPoint (Point p1, Point p2)
{
   Point pt;
   pt.x = (p1.x+p2.x)*0.5;
   pt.y = (p1.y+p2.y)*0.5;
   return pt;
}

inline bool isPointInsideRect (Point pt, COORD_TYPE l, COORD_TYPE t, COORD_TYPE r, COORD_TYPE b)
{
  //Ken:
  //if both a & b are positive and float (not double)
   //if (a < b)
  //if (*(long *)&a < *(long *)&b)
  
  if (pt.x>= l && pt.x<=r && pt.y>= t && pt.y<=b)
    return true;
  else
    return false;
}

inline bool _isTwoRectsIntersecting (COORD_TYPE L1, COORD_TYPE T1, COORD_TYPE R1, COORD_TYPE B1,
                         COORD_TYPE L2, COORD_TYPE T2, COORD_TYPE R2, COORD_TYPE B2)
{
  if (L2<R1 && R2>L1 && T2<B1 && B2>T1)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////
// BASIC GROMETRY: LINE-LINE
// !!WORK!!
//return +1 if p0, p1, p2 is CCW
//return -1 if p0, p1, p2 is CW
//return  0 if p0, p1, p2 is a straight line
inline int _CCW (Point& p0, Point& p1, Point& p2)
{
  float p0x = (float)p0.x;
  float p0y = (float)p0.y;
  float p1x = (float)p1.x;
  float p1y = (float)p1.y;
  float p2x = (float)p2.x;
  float p2y = (float)p2.y;

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

inline bool _isLineLineIntersecting (Point& line1s, Point& line1e, Point& line2s, Point& line2e)
{
  return ( (_CCW(line1s, line1e, line2s)*_CCW(line1s, line1e, line2e)) <=0 ) &&
       ( (_CCW(line2s, line2e, line1s)*_CCW(line2s, line2e, line1e)) <=0 );
}

///////////////////////////////////////////////////////
// BASIC GROMETRY: POINT-LINE

inline double _getT (Point pt, Point lstart, Point lend)
{
   //see matlab/pointline.m
  //Ken: divide is slow.
  return (((double)pt.y-(double)lstart.y)*((double)lend.y-(double)lstart.y) + 
         ((double)pt.x-(double)lstart.x)*((double)lend.x-(double)lstart.x)) /
       (((double)lend.y-(double)lstart.y)*((double)lend.y-(double)lstart.y) + 
        ((double)lend.x-(double)lstart.x)*((double)lend.x-(double)lstart.x));
}

inline DIST_TYPE _distPointLine (Point pt, Point lstart, Point lend)
{
  //Point foot = _getFootPt (pt, lstart, lend);
  //return _distPointPoint (pt, foot);

  //Ken: divide is slow.
  double dx  = (double)lend.x - (double)lstart.x;
  double dy  = (double)lend.y - (double)lstart.y;
  double dxt = (double)pt.x   - (double)lstart.x;
  double dyt = (double)pt.y   - (double)lstart.y;

  return vcl_fabs(dyt*dx - dxt*dy)/ vcl_sqrt(dx*dx + dy*dy);
}

inline DIST_TYPE _distSqPointLine (Point pt, Point lstart, Point lend)
{
  //Point foot = _getFootPt (pt, lstart, lend);
  //return _distSqPointPoint (pt, foot);

  //Ken: divide is slow.
  double dx  = (double)lend.x - (double)lstart.x;
  double dy  = (double)lend.y - (double)lstart.y;
  double dxt = (double)pt.x   - (double)lstart.x;
  double dyt = (double)pt.y   - (double)lstart.y;

  double nom = (dyt*dx - dxt*dy);
  return nom*nom/(dx*dx + dy*dy);
}

inline Point _getFootPt (Point pt, Point lstart, Point lend)
{
  double t = _getT (pt, lstart, lend);

  Point foot;
   foot.x = lstart.x + t* (lend.x-lstart.x);
   foot.y = lstart.y + t* (lend.y-lstart.y);

   return foot;
}

//TO SPEED-UP: AVOID UNNECESSARY ASSIGNMENTS.
inline Point _getFootPt (Point lstart, Point lend, double t)
{
  Point foot;
   foot.x = lstart.x + t* (lend.x-lstart.x);
   foot.y = lstart.y + t* (lend.y-lstart.y);

   return foot;
}

Point _getValidFootPt (Point pt, Point lstart, Point lend);
Point _getValidFootPt (Point pt, Point lstart, Point lend, double t);
VECTOR_TYPE _vPointLine (Point pt, Point lstart, Point lend);
DIST_TYPE _deltaPointLine (Point pt, Point lstart, Point lend);

///////////////////////////////////////////////////////
// BASIC GROMETRY: POINT-ARC
Point getCenterOfArc (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey, 
               double r, ARC_NUD nud, ARC_NUS nus);
Point getArcCenterFromThreePoints (COORD_TYPE x1, COORD_TYPE y1, 
                        COORD_TYPE x2, COORD_TYPE y2,
                        COORD_TYPE x3, COORD_TYPE y3);
double getArcRadiusFromThreePoints(COORD_TYPE x1, COORD_TYPE y1, 
                        COORD_TYPE x2, COORD_TYPE y2,
                        COORD_TYPE x3, COORD_TYPE y3);
double getArcRadiusFromThreePoints (Point Pt1, Point Pt2, Point Pt3);
double getTangentFromThreePoints (COORD_TYPE x1, COORD_TYPE y1, 
                       COORD_TYPE x2, COORD_TYPE y2,
                       COORD_TYPE x3, COORD_TYPE y3);


///////////////////////////////////////////////////////
// BASIC GROMETRY: LINE-ARC


///////////////////////////////////////////////////////
// BASIC GROMETRY: INTERSECTION
bool _isEqPoint (Point pt1, Point pt2, double epsilon);
bool _BisEqPoint (Point pt1, Point pt2);
bool _isPointOnLine (Point pt, Point start, Point end);
bool _isPointOnArc (Point pt, Point start, Point end,
                   Point center, double r, bool nu);


////////////////////////////////////////////////
/*----------------- sh file ------------------*/
#define  MAX_LEN              1024  //Maximum length of line in input file

#define  END_HEADER                      "endheader"
#define  BEGIN_BOUNDARY_INT              "begin boundary_interval" 
#define  END_BOUNDARY_INT                "end boundary_interval"
#define  BEGIN_SHOCK                     "begin shock"
#define  END_SHOCK                       "end shock"

#endif
