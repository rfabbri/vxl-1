#include <vcl_iostream.h>
//#include <stl_config.h>

#ifndef __TBS_POINT_DEF__
#define __TBS_POINT_DEF__
// Defines the point class

template < class elemType >
class Point {
 public:
  //Constructors
  Point(){ _x=0; _y=0; _z=0};
  Point(elemType px, elemType py, elemType pz){_x=px; _y=py; _z=pz;};
  Point(const Point &pt){_x=pt._x; _y=pt._y; _z=pt._z;};

  //Destructor
  ~Point(){};
  elemType x() { return _x;} 
  elemType y() { return _y;} 
  elemType z() { return _z;}

  //Assignment operator
  Point& operator = (const Point& pt){_x=pt._x; _y=pt._y; _z=pt._z; return *this;};
  //Equality relational operator
  bool operator == (const Point& pt){if (_x==pt._x && _y==pt._y && _z==pt._z) return true; else return false;};
  bool operator != (const Point& pt){if (_x!=pt._x || _y!=pt._y || _z!=pt._z) return true; else return false;};


 protected:
  //Data
  elemType _x;
  elemType _y;
  elemType _z;
};

#endif
