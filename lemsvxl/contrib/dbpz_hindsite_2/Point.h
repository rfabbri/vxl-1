
#ifndef __TBS_POINT_DEF__
#define __TBS_POINT_DEF__


template < class elemType >
class PuzPoint {
 public:
  PuzPoint(){ _x=0; _y=0;};
  PuzPoint(elemType px, elemType py){_x=px; _y=py;};
  PuzPoint(const PuzPoint &pt){_x=pt._x; _y=pt._y;};

  ~PuzPoint(){};
  
  elemType x() { return _x;} 
  elemType y() { return _y;} 

  PuzPoint& operator = (const PuzPoint& pt){_x=pt._x; _y=pt._y;return *this;};
  bool operator == (const PuzPoint& pt){if (_x==pt._x && _y==pt._y) return true; else return false;};
  bool operator != (const PuzPoint& pt){if (_x!=pt._x || _y!=pt._y) return true; else return false;};


 protected:
  elemType _x;
  elemType _y;
};

#endif
