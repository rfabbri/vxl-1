#ifndef GEOMNODES_H_INCLUDED
#define GEOMNODES_H_INCLUDED

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_sstream.h>

#include "basegui_nodes.h"
#include "basegui_rgbimage.h"
#include "algo_euler.h"
#include "basegui_xpmath.h"


class LineGeom : public AffineNode {
protected:
  Point2D<double> p0, p1;
public:
  LineGeom(const Point2D<double> &pp0, const Point2D<double> &pp1)
    : p0(pp0), p1(pp1) {}
  LineGeom(double sx, double sy, double ex, double ey) 
    : p0(sx, sy), p1(ex, ey) {}

  Point2D<double> start() const { return p0; }
  void setStart(const Point2D<double> &a) { p0 = a; }
  Point2D<double> end() const { return p1; }
  void setEnd(const Point2D<double> &b) { p1 = b; }
  virtual void draw(SceneGraphVisitor *);

  int getPoints_(vcl_vector<Point2D<double> >& pts_);
  bool setPoint_(int id_, const Point2D<double> pt_);

  virtual int getInformation(vcl_string &info) {
    vcl_ostringstream ostrm;
    ostrm<<"LineGeom: ("<<p0.x()<<", "<<p0.y()<<")-("<<p1.x()<<", "<<p1.y()<<")"<<vcl_endl;
    info=ostrm.str();
    return 0;
  }
};

class EllipseGeom : public AffineNode {
protected:
  double a, b, x, y, t0, t1;
public:
  EllipseGeom(double xx, double yy,
      double aa, double bb,
      double theta0, double theta1) { // in radians
    set(xx, yy, aa, bb, theta0, theta1);
  }
  void set(double xx, double yy,
      double aa, double bb,
      double theta0, double theta1) {
    x = xx;
    y = yy;
    a = aa;
    b = bb;
    t0 = theta0;
    t1 = theta1;
    if(t0 > t1) t1 += 2*M_PI;
  }
  virtual void draw(SceneGraphVisitor *);
};

class EllipseOutlineGeom : public AffineNode {
protected:
  double a, b, x, y, t0, t1;
public:
  EllipseOutlineGeom () {}
  EllipseOutlineGeom(double xx, double yy,
      double aa, double bb,
      double theta0, double theta1) { // in radians
    set(xx, yy, aa, bb, theta0, theta1);
  }
  void set(double xx, double yy,
      double aa, double bb,
      double theta0, double theta1) {
    x = xx;
    y = yy;
    a = aa;
    b = bb;
    t0 = theta0;
    t1 = theta1;
    if(t0 > t1) t1 += 2*M_PI;
  }
  virtual void draw(SceneGraphVisitor *);

  double getX () { return x; }
  double getY () { return y; }
  double getA () { return a; }
  double getB () { return b; }
  double getT0 () { return t0; }
  double getT1 () { return t1; }

  int getPoints_(vcl_vector<Point2D<double> >& pts_);
  bool setPoint_(int id_, const Point2D<double> pt_);
};

class ArcGeom : public EllipseOutlineGeom
{
protected:
  Point2D<double> Start, End, Center;
  double r;
  int nud;
public:
  ArcGeom () {}
  ArcGeom(  double sx, double sy, double ex, double ey,
    double cx, double cy, double R, int nud);
  ArcGeom(  double start_angle, double end_angle, double cx, double cy,
    double R, int nud);
    
  void setParameters(double sx, double sy, double ex, double ey,
    double cx, double cy, double R, int nud);

  Point2D<double> start () { return Start; }
  Point2D<double> end () { return End; }
  Point2D<double> center () { return Center; }
  double R () { return r; }
  int nuD () { return nud; }
  virtual int getInformation(vcl_string &info) {
    vcl_ostringstream ostrm;
    ostrm<<"ArcGeom: ("<<Start.x()<<", "<<Start.y()<<")-("<<End.x()<<", "<<End.y()<<")";
    ostrm<<" center: ("<<Center.x()<<", "<<Center.y()<<"), R: "<<r<<", nud: "<<nud<<vcl_endl;
    info=ostrm.str();
    return 0;
  }
};

class PointGeom : public AffineNode {
protected:
  double x, y;
public:
  PointGeom(double xx, double yy) : x(xx), y(yy) {}
  PointGeom(const Point2D<double> &a) : x(a.getX()), y(a.getY()) {}

  void setPoint(const Point2D<double> &a) { x = a.getX(); y = a.getY(); }
  Point2D<double> point() const { return Point2D<double>(x,y); }
  virtual void draw(SceneGraphVisitor *);
  virtual int getInformation(vcl_string &info) {
    vcl_ostringstream ostrm;
    ostrm<<"PointGeom: ("<<x<<", "<<y<<")"<<vcl_endl;
    info=ostrm.str();
    return 0;
  }
};

// non-closed sequence of lines
class PolylineGeom : public AffineNode {
protected:
  vcl_vector<Point2D<double> > _points;

public:
  PolylineGeom() : AffineNode() {}
    PolylineGeom(const vcl_vector<Point2D<double> > &pts)
      {
       for (unsigned i=0;i<pts.size();i++)
         {
           _points.push_back(pts[i]);
          };
       }

    virtual ~PolylineGeom(){};
    
  virtual void push_back(double x, double y) { _points.push_back(Point2D<double>(x,y)); }
  virtual void push_back(const Point2D<double> &d) { _points.push_back(d); }
  virtual void pop_back() { _points.pop_back(); }
  virtual void erase() { _points.erase(_points.begin(), _points.end()); }

  size_t size() const { return _points.size(); }
  Point2D<double> point(size_t i) const { assert(i >= 0 && i < _points.size()); return _points[i]; }
  virtual void setPoint(size_t i, const Point2D<double> &a) {
    assert(i >= 0 && i < _points.size());
    _points[i] = a;
  }
  virtual void draw(SceneGraphVisitor *);

  int getPoints_(vcl_vector<Point2D<double> >& pts_);
  bool setPoint_(int id_, const Point2D<double> pt_);
};

class UnfilledPolygonGeom : public PolylineGeom {
public:
  // The constructor argument is fake.
  // It's there to make Rectangle template work.
  UnfilledPolygonGeom
    (const Point2D<double>& op_=Point2D<double>((double)0,(double)0))
    : PolylineGeom()
    {}
  virtual void draw(SceneGraphVisitor *);
};

class ConvexPolygonGeom: public PolylineGeom { // filled, must be convex
public:
  ConvexPolygonGeom() {}
  virtual void draw(SceneGraphVisitor *);
};

class FilledPolygonGeom: public PolylineGeom { // arbitrary (convex or concave) polygon
public:
  // The constructor argument is fake.
  // It's there to make Rectangle template work.`
  FilledPolygonGeom
    (const Point2D<double>& op_=Point2D<double>((double)0,(double)0))
    : PolylineGeom()
    {}

    FilledPolygonGeom (const vcl_vector<Point2D<double> > points): 
    PolylineGeom(){ _points = points;}

  virtual void draw(SceneGraphVisitor *);
};

class TextGeom: public GeometryNode {
  float x, y;
  vcl_string txt;
public:
  TextGeom(float xx, float yy, vcl_string tt) : x(xx), y(yy), txt(tt) {}
  void setPoint(const Point2D<double> &a) { x = a.getX(); y = a.getY(); }
  Point2D<double> point() const { return Point2D<double>(x, y); }
  void setText(const vcl_string a) { txt = a; }
  vcl_string text() const { return txt; }
  virtual void draw(SceneGraphVisitor *);
};

class AffineTextGeom: public AffineNode {
  float x, y;
  vcl_string txt;
public:
  AffineTextGeom(Point2D<double> pos, vcl_string tt, bool flip = false);
  AffineTextGeom(float xx, float yy, vcl_string tt, bool flip = false);
  void setPoint(const Point2D<double> &a) { x = a.getX(); y = a.getY(); }
  Point2D<double> point() const { return Point2D<double>(x, y); }
  void setText(const vcl_string a) { txt = a; }
  vcl_string text() const { return txt; }
  virtual void draw(SceneGraphVisitor *);
};

class ImageGeom: public AffineNode 
{
  Image *img;
  double x1, y1, width, height;
    
    public:
    Image* getImage() const
      {
        return img;
       }
    
    ImageGeom(Image *ii, double x1, double y1, double w,                  bool flip = false);
    ImageGeom(Image *ii, double x1, double y1, double w, double h,        bool flip = false);
    ImageGeom(vcl_string file_name, double x1, double y1,                     bool flip = false);
    ImageGeom(vcl_string file_name, double x1, double y1, double w,           bool flip = false);
    ImageGeom(vcl_string file_name, double x1, double y1, double w, double h, bool flip = false);
    
    virtual void draw(SceneGraphVisitor *);
   
    virtual int getInformation(vcl_string &info, Point2D<double> pos);
 };

// scale invariant nodes

class ScaleInvariantEllipseGeom : public ScaleInvariantNode {
protected:
  double a, b, t0, t1;
public:
  ScaleInvariantEllipseGeom(double xx, double yy,
      double aa, double bb,
      double theta0, double theta1) : ScaleInvariantNode(xx,yy) { // in radians
    set(xx, yy, aa, bb, theta0, theta1);
  }
  void set(double xx, double yy,
      double aa, double bb,
      double theta0, double theta1) {
    setOrigin(xx, yy);
    a = aa;
    b = bb;
    t0 = theta0;
    t1 = theta1;
    if(t0 > t1) t1 += 2*M_PI;
  }
  virtual void draw(SceneGraphVisitor *);
};

class ScaleInvariantEllipseOutlineGeom : public ScaleInvariantNode {
protected:
  double a, b, t0, t1;
public:
  ScaleInvariantEllipseOutlineGeom(double xx, double yy,
      double aa, double bb,
      double theta0, double theta1) : ScaleInvariantNode(xx,yy) { // in radians
    set(xx, yy, aa, bb, theta0, theta1);
  }
  void set(double xx, double yy,
      double aa, double bb,
      double theta0, double theta1) {
    setOrigin(xx, yy);
    a = aa;
    b = bb;
    t0 = theta0;
    t1 = theta1;
    if(t0 > t1) t1 += 2*M_PI;
  }
  virtual void draw(SceneGraphVisitor *);
};

// non-closed sequence of lines
class ScaleInvariantPolylineGeom : public ScaleInvariantNode {
public:
  typedef vcl_vector<Point2D<double> > points_t;
protected:
  points_t _points;

public:
  ScaleInvariantPolylineGeom(double ox, double oy) : ScaleInvariantNode(ox, oy) {}
  ScaleInvariantPolylineGeom(const Point2D<double> &origin) : ScaleInvariantNode(origin) {}

  void push_back(double x, double y) { _points.push_back(Point2D<double>(x,y)); }
  void push_back(const Point2D<double> &d) { _points.push_back(d); }
  void pop_back() { _points.pop_back(); }
  void erase() { _points.erase(_points.begin(), _points.end()); }

  size_t size() const { return _points.size(); }
  Point2D<double> point(size_t i) const { assert(i >= 0 && i < _points.size()); return _points[i]; }
  void setPoint(size_t i, const Point2D<double> &a) {
    assert(i >= 0 && i < _points.size());
    _points[i] = a;
  }
  virtual void draw(SceneGraphVisitor *);
};

class ScaleInvariantConvexPolygonGeom: public ScaleInvariantPolylineGeom { 
  // filled, must be convex
public:
  ScaleInvariantConvexPolygonGeom(double ox, double oy) : ScaleInvariantPolylineGeom(ox, oy) {}
  ScaleInvariantConvexPolygonGeom(const Point2D<double> &origin) : ScaleInvariantPolylineGeom(origin) {}

  virtual void draw(SceneGraphVisitor *);
};

class ScaleInvariantFilledPolygonGeom: public ScaleInvariantPolylineGeom {
  // arbitrary (convex or concave) polygon
public:
  ScaleInvariantFilledPolygonGeom(double ox, double oy) : ScaleInvariantPolylineGeom(ox, oy) {}
  ScaleInvariantFilledPolygonGeom(const Point2D<double> &origin) : ScaleInvariantPolylineGeom(origin) {}

  virtual void draw(SceneGraphVisitor *);
};

//Ming: move ShockArrowGeom to here.

class ShockArrowGeom : public ScaleInvariantFilledPolygonGeom 
{
public :
  ShockArrowGeom (double x, double y, double dir, double ARROW_SIZE=5, double ARROW_ANGLE=0.4);
  ~ShockArrowGeom(){};
};

class ArrowHeadGeom :public FilledPolygonGeom
  {
    public :
        ArrowHeadGeom(Point2D<double> pt_, double angle_, double totalLength_=30);
        ~ArrowHeadGeom(){};

       void setDirection(double theta);
       void setPosition(Point2D<double> );
       double getDirection();
               
   };

class ArrowGeom : public AffineNode
  {
    private :
        LineGeom      *_line;
        ArrowHeadGeom *_head;
      double _mag;
      double _headmag;

    public:

        ArrowGeom()
          {
            _line=0;
            _head=0;
           }

        ArrowGeom(Point2D<double> start, Point2D<double> end)
          {
            _line = new LineGeom(start, end);
            _head = new ArrowHeadGeom(end, atan2(end.getY()-start.getY(), end.getX()-start.getX()), 2);
           } 

       ArrowGeom(Point2D<double> start, double angle, double mag =20, double headmag=2)
          {
        _mag = mag;
        _headmag = headmag;

            angle = fmod(angle, 2*M_PI);
            if (angle<0)
                angle +=2*M_PI;
            
            Point2D<double> end;
            
            end.setX(_mag*cos(angle)+start.getX());
            end.setY(_mag*sin(angle)+start.getY());
            
            _line = new LineGeom(start, end);
            _head = new ArrowHeadGeom(end, angle, _headmag);
           }

        ~ArrowGeom()
          {
            delete _line;
            _line = 0;
            delete _head;
            _head = 0;
           }

        ArrowGeom(const ArrowGeom& rhs)
          {
            _line = rhs._line;
            _head = rhs._head;
           }

        ArrowGeom& operator=(const ArrowGeom &rhs)
          {
            if (this!=&rhs)
              {
                _line = rhs._line;
                _head = rhs._head;
               }
            return *this;
           }

      void set(Point2D<double> start, double angle)
      {
        angle = fmod(angle, 2*M_PI);
            if (angle<0)
                angle +=2*M_PI;
            
            Point2D<double> end;
            
            end.setX(_mag*cos(angle)+start.getX());
            end.setY(_mag*sin(angle)+start.getY());

        _line->setStart(start);
        _line->setEnd(end);
        _head->setPosition(end);
            _head->setDirection(angle);
      }

        void setEnd(Point2D<double> end)
          {
            if ((_line)&&(_head))
              {
                _line->setEnd(end);  
                _head->setPosition(end);
                _head->setDirection(atan2(end.getY()-_line->start().getY(),end.getX()-_line->start().getX()));
               }
           }
   
        virtual void draw(SceneGraphVisitor *visitor)
          {
            if (_line)
                _line->draw(visitor);
            if (_head)
                _head->draw(visitor);
           }

        Point2D<double> getStart()
          {
            if (_line)
                return _line->start();
            else
                return Point2D<double>(-1,-1);
           }

        double getDirection()
          {
            if (_head)
                return _head->getDirection();
            else
                return 0;
           }
   
   };

class EulerSpiralSplineGeom :public PolylineGeom 
  {
    private:
        vcl_vector<vcl_pair<Point2D<double>, double> > _pointTangents;
        vcl_vector<EulerSpiralParams> _params;
    public:
        EulerSpiralSplineGeom(){};

        ~EulerSpiralSplineGeom(){};
        
        EulerSpiralSplineGeom(const EulerSpiralSplineGeom &rhs)
          {
            _pointTangents = rhs._pointTangents;
            _params        = rhs._params;
           }
  
       EulerSpiralSplineGeom& operator=(const EulerSpiralSplineGeom &rhs)
         {
           if (this!=&rhs)
             {
               _pointTangents.clear();

               _pointTangents=rhs._pointTangents;
               _params.clear();
               _params=rhs._params;
              }
           return *this;
          }
 
       void computeParams()
         {
           if (_pointTangents.size()==0)
               return;

           if (_params.size()<(_pointTangents.size()-1))
             {
               EulerSpiral ES;
               int start = _params.size();
               int end   = _pointTangents.size()-1;
               for (int i=start;i<end;i++)
                 {
                   ES.set_params(_pointTangents[i].first, _pointTangents[i].second,_pointTangents[i+1].first, _pointTangents[i+1].second);  
                   _params.push_back(ES.getParams());
                  }
              }
           
          }

       virtual void draw(SceneGraphVisitor *visitor);
       
       void push_back(ArrowGeom *arrow)
         {
           Point2D<double> pos  = arrow->getStart(); 
           double         theta = arrow->getDirection();
           _pointTangents.push_back(vcl_pair<Point2D<double>, double>(pos, theta));
          }

      void push_back(vcl_pair<Point2D<double>, double>pt_tangent)
         {
           _pointTangents.push_back(pt_tangent);
          }
       
       void pop_back()
         {
           _pointTangents.pop_back();
          }

       void clear()
     {
       _pointTangents.clear();
       _params.clear();
     }
   };



//
//Utility
//

class EllipseInfo
  {
    private:
        Point2D<double> _center;
        double _a, _b;
        double _startTheta, _endTheta;
    public:
        EllipseInfo(Point2D<double> c_, double a_, double b_, double startTheta_, double endTheta_)
          {
            _center      = c_;
            _a           = a_;
            _b           = b_;
            _startTheta = startTheta_;
            _endTheta   = endTheta_;
           }
       
        EllipseInfo()
          {
            _a=_b=_startTheta=_endTheta=0;
           }
        ~EllipseInfo(){};

        EllipseInfo(const EllipseInfo& old)
          {
            _center      = old._center;
            _a           = old._a;
            _b           = old._b;
            _startTheta  = old._startTheta;
            _endTheta    = old._endTheta;
           }
  
        EllipseInfo& operator=(const EllipseInfo& rhs)
          {
            if (this!=&rhs)
              {
                _center      = rhs._center;
                _a           = rhs._a;
                _b           = rhs._b;
                _startTheta  = rhs._startTheta;
                _endTheta    = rhs._endTheta;
               }
            return *this;
           }
   
       //
       //Get Functions
       //

        Point2D<double> getCenter() const
          {
            return _center;
           }

        double getA() const
          {
            return _a;
           }
        
        double getB() const
          {
            return _b;
           }

        double getStartTheta() const
          {
            return _startTheta;
           }

        double getEndTheta() const
          {
            return _endTheta;
           }

        //
        //Set functions
        //

        void setCenter(Point2D<double> c)
          {
            _center=c;
           }

        void setA(double a)
          {
             _a=a;
           }
        
        void setB(double b)
          {
            _b=b;
           }

        void setStartTheta(double theta)
          {
            _startTheta=theta;
           }

        void setEndTheta(double theta)
          {
             _endTheta=theta;
           }
   
   };

class BiArcSplineGeom :public AffineNode 
  {
    private:
        vcl_vector<vcl_pair<Point2D<double>, double> > _pointTangents;
        vcl_vector<BiArcParams> _params;
        vcl_vector<EllipseInfo> _biarcs;
    public:
        BiArcSplineGeom(){};

        ~BiArcSplineGeom(){};
        
        BiArcSplineGeom(const BiArcSplineGeom &rhs)
          {
            _pointTangents = rhs._pointTangents;
            _params        = rhs._params;
            _biarcs        = rhs._biarcs;
           }
  
       BiArcSplineGeom& operator=(const BiArcSplineGeom &rhs)
         {
           if (this!=&rhs)
             {
               _pointTangents.clear();
               _pointTangents=rhs._pointTangents;
 
               _params.clear();
               _params=rhs._params;

               _biarcs.clear();
               _biarcs=rhs._biarcs;
              }
           return *this;
          }
 
       void computeParams();

       virtual void draw(SceneGraphVisitor *visitor);
       
       void push_back(ArrowGeom *arrow)
         {
           Point2D<double> pos  = arrow->getStart(); 
           double         theta = arrow->getDirection();
           _pointTangents.push_back(vcl_pair<Point2D<double>, double>(pos, theta));
          }

        void push_back(vcl_pair<Point2D<double>, double>pt_tangent)
         {
           _pointTangents.push_back(pt_tangent);
          }

       void pop_back()
         {
           _pointTangents.pop_back();
          }

       void clear()
     {
       _pointTangents.clear();
          _params.clear();
       _biarcs.clear();
     }
   };

#endif
