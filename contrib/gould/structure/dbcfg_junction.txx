//      dbcfg_junction.txx
#ifndef dbcfg_junction_txx_
#define dbcfg_junction_txx_

// Template for contour fragment graph's junctions
// Benjamin Gould
// 7/15/09

#include "dbcfg_curve.h"
#include <vcl_deque.h>
#include <vgl/vgl_point_2d.h>

// create an unconnected junction
template <class T>
dbcfg_junction<T>::dbcfg_junction(T x, T y) :
_x(x),
_y(y) {
}

// create a junction connected to a vector of curves
template <class T>
dbcfg_junction<T>::dbcfg_junction(T x, T y, vcl_vector<dbcfg_curve<T> * > curves) :
_x(x),
_y(y),
_curves(curves) {
}

// destruct the junction
template <class T>
dbcfg_junction<T>::~dbcfg_junction() {
}

// returns the x-coordinate of the junction
template <class T>
inline
T dbcfg_junction<T>::x() const {
  return _x;
}

// returns the y-coordinate of the junction
template <class T>
inline
T dbcfg_junction<T>::y() const {
  return _y;
}

// returns the curves in the junction
template <class T>
inline
vcl_vector<dbcfg_curve<T> * > dbcfg_junction<T>::get_curves() {
  return _curves;
}

// returns the curves in the junction that exist at the given depth
template <class T>
vcl_vector<dbcfg_curve<T> * > dbcfg_junction<T>::get_curves(int depth) {
  vcl_vector<dbcfg_curve<T> * > curves;
  for (vcl_vector<dbcfg_curve<T> * >::iterator iter = _curves.begin(); iter < _curves.end(); iter++) {
    if ((*iter)->exists_at(depth)) {
      curves.push_back(*iter);
    }
  }
  return curves;
}

// returns true if the junction exists (connects to more than one curve) at the given depth
template <class T>
bool dbcfg_junction<T>::exists_at(int depth) {
  int count = 0;
  for (vcl_vector<dbcfg_curve<T> * >::iterator iter = _curves.begin(); iter < _curves.end(); iter++) {
    if ((*iter)->exists_at(depth)) {
      count++;
    }
  }
  return count > 1;
}

// add a new curve to the connection
// returns true if successful
template <class T>
bool dbcfg_junction<T>::connect_curve(dbcfg_curve<T> * curve) {
  if (this->can_connect(curve) && !this->contains_curve(curve)) {
    _curves.push_back(curve);
    return true;
  }
  return false;
}

// removes a curve from the connection
// returns true if successful
template <class T>
bool dbcfg_junction<T>::disconnect_curve(dbcfg_curve<T> * curve) {
  for (vcl_vector<dbcfg_curve<T> * >::iterator iter = _curves.begin(); iter < _curves.end(); iter++) {
    if (curve == *iter) {
      _curves.erase(iter);
      return true;
    }
  }
  return false;
}

// returns true if the junction contains this curve
template <class T>
bool dbcfg_junction<T>::contains_curve(dbcfg_curve<T> * curve) {
  for (vcl_vector<dbcfg_curve<T> * >::iterator iter = _curves.begin(); iter < _curves.end(); iter++) {
    if (curve == *iter) {
      return true;
    }
  }
  return false;
}

// returns true if the junction lies on the curve
template <class T>
bool dbcfg_junction<T>::can_connect(dbcfg_curve<T> * curve) {
  vcl_deque<dbdet_edgel * >* edgels = &(curve->get_edgel_chain()->edgels);
  for (vcl_deque<dbdet_edgel * >::iterator iter = edgels->begin(); iter < edgels->end(); iter++) {
    vgl_point_2d<double> pt = (*iter)->pt;
    if (pt.x() == _x && pt.y() == _y) {
      return true;
    }
  }
  return false;
}


// -------------------------
// INSTANTIATION
// -------------------------

#define DBCFG_JUNCTION_INSTANTIATE(T) \
template class dbcfg_junction<T>;\

#endif dbcfg_junction_txx_
// end dbcfg_junction.txx

