#ifndef XFORM_H_INCLUDED
#define XFORM_H_INCLUDED

#include <assert.h>
#include <math.h>

class XForm3x3;
class Vect3;

typedef XForm3x3 XForm;
typedef Vect3 Vect;

XForm3x3 operator*(const XForm3x3 &x, const XForm3x3 &y);

class Vect3 {
protected:
  enum { R = 3 };
  double vv[R];

public:
  Vect3() {
    for(int i = 0; i < R; ++i)
      vv[i] = 0;
  }
  Vect3(double a, double b, double c) {
    vv[0] = a;
    vv[1] = b;
    vv[2] = c;
  }
  Vect3(const double *f) { set(f); }
  Vect3(const Vect3 &mm) { set(mm.vv); }

  Vect3 &operator=(const Vect3 &mm) {
    set(mm.vv);
    return *this;
  }
  void set(const double *f) {
    memcpy(vv, f, sizeof(double)*R);
  }

  double operator[](int i) const {
    assert(i < R);
    return vv[i];
  }
  double &operator[](int i) {
    assert(i < R);
    return vv[i];
  }

  const double *data() const { return vv; }
  double *data() { return vv; }

  Vect3 &operator *=(double f) {
    for(int i = 0; i < R; ++i) vv[i] *= f;
    return *this;
  }
  Vect3 &operator /=(double f) {
    for(int i = 0; i < R; ++i) vv[i] /= f;
    return *this;
  }

  Vect3 &operator +=(const Vect3 &x) {
    for(int i = 0; i < R; ++i) vv[i] += x.vv[i];
    return *this;
  }
  Vect3 &operator -=(const Vect3 &x) {
    for(int i = 0; i < R; ++i) vv[i] -= x.vv[i];
    return *this;
  }

  void clear() { for(int i = 0; i < R; ++i) vv[i] = 0; }
};

inline Vect3 operator*(const Vect3 &x, double f) { 
  Vect3 m(x);
  m *= f;
  return m;
};

inline Vect3 operator*(double f, const Vect3 &x) { return x*f; };

inline Vect3 operator/(const Vect3 &x, double f) {
  Vect3 m(x);
  m /= f;
  return m;
};

inline Vect3 operator-(const Vect3 &x) {
  Vect3 m;
  for(int i = 0; i < 3; ++i)
    m[i] = -x[i];
  return m;
};

inline Vect3 operator+(const Vect3 &x) {
  return x;
};

inline Vect3 operator-(const Vect3 &x, const Vect3 &y) {
  Vect3 m;
  for(int i = 0; i < 3; ++i)
    m[i] = x[i] - y[i];
  return m;
};

inline Vect3 operator+(const Vect3 &x, const Vect3 &y) {
  Vect3 m;
  for(int i = 0; i < 3; ++i)
    m[i] = x[i] + y[i];
  return m;
};

inline double dot(const Vect3 &a, const Vect3 &b) {
  double sum = 0;
  for(int i = 0; i < 3; ++i)
    sum += a[i]*b[i];
  return sum;
};

inline double len2(const Vect3 &m) {
  return dot(m, m);
};

inline double length(const Vect3 &m) { return sqrt(len2(m)); };

inline double dist(const Vect3 &a, const Vect3 &b) { return length(a-b); };

inline Vect3 unit(const Vect3 &v) {
  double len = length(v);
  return v/len;
};

class XForm3x3 {
public:
  enum { R = 3, C = 3 };
  double vv[R*C];

public:
    double transx, transy, rotAngle;

    void print(){
        for( int i = 0; i < R; i++ ){
            vcl_cout << "[";
            for( int j = 0; j  < C; j++ ){
                vcl_cout << " " << (*this)(i,j);
            }
            vcl_cout << " ]" << vcl_endl;
        }
    }


  XForm3x3() {
        transx = 0; transy = 0; rotAngle = 0;
    for(int i = 0; i < R; ++i)
      for(int j = 0; j < C; ++j)
        (*this)(i, j) = (i == j);
  }
  XForm3x3(const double *f) { set(f); }
  XForm3x3(const XForm3x3 &mm) { set(mm.vv); }

  XForm3x3 &operator=(const XForm3x3 &mm) {
    set(mm.vv);
    return *this;
  }
  void set(const double *f) {
    memcpy(vv, f, sizeof(double)*R*C);
  }


  double operator()(int i, int j) const {
    assert(i < R && j < C);
    return vv[i*C + j];
  }
  double &operator()(int i, int j) {
    assert(i < R && j < C);
    return vv[i*C + j];
  }

  const double *data() const { return vv; }
  double *data() { return vv; }

  XForm3x3 &operator *=(double f) {
    for(int i = 0; i < R*C; ++i) vv[i] *= f;
    return *this;
  }
  XForm3x3 &operator /=(double f) {
    for(int i = 0; i < R*C; ++i) vv[i] /= f;
    return *this;
  }

  XForm3x3 &operator +=(const XForm3x3 &x) {
    for(int i = 0; i < R*C; ++i) vv[i] += x.vv[i];
    return *this;
  }
  XForm3x3 &operator -=(const XForm3x3 &x) {
    for(int i = 0; i < R*C; ++i) vv[i] -= x.vv[i];
    return *this;
  }
  XForm3x3 &operator *=(const XForm3x3 &x) {
    XForm3x3 tmp;
    tmp = (*this) * x;
    *this = tmp;
    return *this;
  }

  void clear() { for(int i = 0; i < R*C; ++i) vv[i] = 0; }

 //TEST
  void translateMe( double x, double y )
  {
    vv[0*C + 2] = x; transx = x; 
    vv[1*C + 2] = y; transy = y;
  }
  
  void rotateMe( double angle )
  { //radians
    rotAngle = angle;
    vv[0*C + 0] = cos(angle); vv[0*C + 1] =-sin(angle);
    vv[1*C + 0] = sin(angle); vv[1*C + 1] = cos(angle);
  }
  //TEST

  static XForm3x3 identity() {
    XForm3x3 m;
    for(int i = 0; i < R; ++i)
      for(int j = 0; j < C; ++j)
        m(i, j) = (i == j);
    return m;
  }

  static XForm3x3 uniform_scale(double v) {
    XForm3x3 m = XForm3x3::identity();
    for(int i = 0; i < 2; ++i) m(i, i) = v;
    return m;
  }

  static XForm3x3 rotation(double angle) { //radians
    XForm3x3 m = XForm3x3::identity();
        m.rotAngle = angle;
    m(0,0) = cos(angle); m(0,1) =-sin(angle);
    m(1,0) = sin(angle); m(1,1) = cos(angle);
    return m;
  }

  static XForm3x3 translation(const Vect3 &p) {
    XForm3x3 m = XForm3x3::identity();
    m(0,2) = p[0]; m.transx = p[0];
    m(1,2) = p[1]; m.transy = p[1];
    return m;
  }

  static XForm3x3 translation(double x, double y) {
    return translation(Vect3(x,y,0));
  }

  static XForm3x3 reflect_y_axis() {
    XForm3x3 m = XForm3x3::identity();
    m(1,1) = -1; // reflect in the y axis
    return m;
  }

  static void mult( XForm3x3 &x, const XForm3x3 &y) 
  {
    XForm3x3 temp;// CAN added
    for(int r = 0; r < 3; ++r)
    {
      for(int c = 0; c < 3; ++c) 
        {
          double sum = 0;
          for(int i = 0; i < 3; ++i)
            sum += x(r, i) * y(i, c);
          temp(r, c) = sum; // CAN added
//          x(r, c) = sum; // CAN removed
        }
      }
    // CAN added
    temp.rotAngle = x.rotAngle + y.rotAngle;
    temp.transx = x.transx + y.transx;
    temp.transy = x.transy + y.transy;
    // I have to do the copying here because deep copy function needs pointer
    // for the first argument, but I have reference here.
    for(int r = 0; r < 3; ++r)
      for(int c = 0; c < 3; ++c)
        x(r,c) = temp(r,c);
    x.transx = temp.transx;
    x.transy = temp.transy;
    x.rotAngle = temp.rotAngle;
    // CAN added

    // CAN removed
//    x.rotAngle = x.rotAngle + y.rotAngle;
//    x.transx = x.transx + y.transx;
//    x.transy = x.transy + y.transy;
    // CAN removed
  }

  static void deep_copy( XForm3x3 *x, XForm3x3 &y)
  {
    for(int r = 0; r < 3; ++r)
      for(int c = 0; c < 3; ++c)
        (*x)(r,c) = y(r,c);
    x->transx = y.transx;
    x->transy = y.transy;
    x->rotAngle = y.rotAngle;
  }
};


inline bool operator == (const XForm3x3 &x, const XForm3x3 &y) {
  //XForm3x3 m;
  for(int r = 0; r < 3; ++r)
    for(int c = 0; c < 3; ++c)
      if(x(r,c)!=y(r,c))
        return false;
  return true;
};



inline XForm3x3 operator*(const XForm3x3 &x, const XForm3x3 &y) {
  XForm3x3 m;
  for(int r = 0; r < 3; ++r) {
    for(int c = 0; c < 3; ++c) {
      double sum = 0;
      for(int i = 0; i < 3; ++i)
        sum += x(r, i) * y(i, c);
      m(r, c) = sum;
    }
  }
    m.rotAngle = x.rotAngle + y.rotAngle;
    m.transx = x.transx + y.transx;
    m.transy = x.transy + y.transy;
  return m;
};

inline Vect3 operator*(XForm3x3 a, const Vect3 &x) {
  Vect3 b;
  for(int i = 0; i < 3; ++i) {
    double sum = 0;
    for(int j = 0; j < 3; ++j) {
      sum += a(i,j)*x[j];
    }
    b[i] = sum;
  }
  return b;
};

inline XForm3x3 transpose(const XForm3x3 &x) {
  XForm3x3 m;
  for(int i = 0; i < 3; ++i)
    for(int j = 0; j < 3; ++j)
      m(j, i) = x(i, j);
  return m;
};

inline double determinant(const XForm3x3 &m) {
  return 
      m(0,0)*m(1,1)*m(2,2)
    - m(0,0)*m(1,2)*m(2,1)
    - m(1,0)*m(0,1)*m(2,2)
    + m(1,0)*m(0,2)*m(2,1)
    + m(2,0)*m(0,1)*m(1,2)
    - m(2,0)*m(0,2)*m(1,1);
};

inline XForm3x3 invert(const XForm3x3 &x) {
  XForm3x3 r;
  double det = determinant(x);

  // never question the infinite wisdom of that which is called... maple
  r(0,0) =  (x(1,1)*x(2,2)-x(1,2)*x(2,1))/det;
  r(0,1) = -(x(0,1)*x(2,2)-x(0,2)*x(2,1))/det;
  r(0,2) =  (x(0,1)*x(1,2)-x(0,2)*x(1,1))/det;
  r(1,0) = -(x(1,0)*x(2,2)-x(1,2)*x(2,0))/det;
  r(1,1) =  (x(0,0)*x(2,2)-x(0,2)*x(2,0))/det;
  r(1,2) = -(x(0,0)*x(1,2)-x(0,2)*x(1,0))/det;
  r(2,0) =  (x(1,0)*x(2,1)-x(1,1)*x(2,0))/det;
  r(2,1) = -(x(0,0)*x(2,1)-x(0,1)*x(2,0))/det;
  r(2,2) =  (x(0,0)*x(1,1)-x(0,1)*x(1,0))/det;
  return r;
};
#endif
