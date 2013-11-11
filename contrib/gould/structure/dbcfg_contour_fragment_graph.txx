//      dbcfg_contour_fragment_graph.txx
#ifndef dbcfg_contour_fragment_graph_txx_
#define dbcfg_contour_fragment_graph_txx_

// Template for contour fragment graph
// Benjamin Gould
// 7/20/09

#include "dbcfg_contour_fragment_graph.h"
#include <vcl_deque.h>
#include <vgl/vgl_point_2d.h>



// creates an empty contour fragment graph with the given bounds
// this is more efficient than the boundless contructor
template <class T>
dbcfg_contour_fragment_graph<T>::dbcfg_contour_fragment_graph(int xsize, int ysize, T epsilon) :
_boundless(false),
_xsize(xsize),
_ysize(ysize),
_epsilon(epsilon) {
  _curve_matrix = vnl_matrix<vcl_vector<dbcfg_cfg_object_location<dbcfg_curve<T> *, T> > * >(_xsize, _ysize, false);
  _junction_matrix = vnl_matrix<vcl_vector<dbcfg_junction<T> * > * >(_xsize, _ysize, false);
}

// creates an empty contour fragment graph with no bounds
template <class T>
dbcfg_contour_fragment_graph<T>::dbcfg_contour_fragment_graph(T epsilon) :
_boundless(true),
_xsize(200),
_ysize(200),
_epsilon(epsilon) {
  _curve_matrix = vnl_matrix<vcl_vector<dbcfg_cfg_object_location<dbcfg_curve<T> *, T> > * >(_xsize, _ysize, false);
  _junction_matrix = vnl_matrix<vcl_vector<dbcfg_junction<T> * > * >(_xsize, _ysize, false);
}

// destruct the contour fragment graph
template <class T>
dbcfg_contour_fragment_graph<T>::~dbcfg_contour_fragment_graph() {
  this->clean_matrices();
}

// returns the maximum x value (bound) or 0 if boundless
// [use 0 instead of -1 to allows for T being unsigned]
template <class T>
inline
T dbcfg_contour_fragment_graph<T>::xmax() {
  return _boundless ? 0 : (T) _xsize;
}

// returns the maximum y value (bound) or 0 if boundless
// [use 0 instead of -1 to allows for T being unsigned]
template <class T>
inline
T dbcfg_contour_fragment_graph<T>::ymax() {
  return _boundless ? 0 : (T) _ysize;
}

// returns the maximum x value (bound) or the greatest x value if boundless
template <class T>
inline
T dbcfg_contour_fragment_graph<T>::width() {
  return _boundless ? _xmax : (T) _xsize;
}

// returns the maximum y value (bound) or the greatest y value if boundless
template <class T>
inline
T dbcfg_contour_fragment_graph<T>::height() {
  return _boundless ? _ymax : (T) _ysize;
}

// resets the cfg
template <class T>
void dbcfg_contour_fragment_graph<T>::reset(int xsize, int ysize) {
  this->clean_matrices();
  _boundless = false;
  _xsize = xsize;
  _ysize = ysize;
  _curves.clear();
  _junctions.clear();
  _curve_matrix.set_size(xsize, ysize);
  _junction_matrix.set_size(xsize, ysize);
}

// add a new curve to the cfg, populating junctions as needed
// because higher level curves are built on lower level ones, curves should be
// created from the bottom up (highest depth to lowest depth)
template <class T>
void dbcfg_contour_fragment_graph<T>::add_curve(dbcfg_curve<T>* curve) {
  vcl_deque<dbdet_edgel* >* edgels = &(curve->get_edgel_chain()->edgels);
  for (vcl_deque<dbdet_edgel* >::iterator iter = edgels->begin(); iter < edgels->end(); iter++) {
    vgl_point_2d<double> pt = (*iter)->pt;
    int ix, iy;
    internal_xy((T) pt.x(), (T) pt.y(), ix, iy);
    vcl_vector<dbcfg_cfg_object_location<dbcfg_curve<T> *, T> >* curve_bucket = _curve_matrix(ix, iy);
    if (curve_bucket == false) {
      _curve_matrix(ix, iy) = new vcl_vector<dbcfg_cfg_object_location<dbcfg_curve<T> *, T> >();
      curve_bucket = _curve_matrix(ix, iy);
    }
    for (vcl_vector<dbcfg_cfg_object_location<dbcfg_curve<T> *, T> >::iterator citer = curve_bucket->begin(); citer < curve_bucket->end(); citer++) {
      if ((*citer).exists_at(pt.x(), pt.y(), curve->min_depth(), curve->max_depth())) {
        dbcfg_junction<T>* junction = (*citer).get_object()->intersect(curve, pt.x(), pt.y());
        if (junction != false) {
          vcl_vector<dbcfg_junction<T> * >* junction_bucket = _junction_matrix(ix, iy);
          if (junction_bucket == false) {
            _junction_matrix(ix, iy) = new vcl_vector<dbcfg_junction<T> * >();
            junction_bucket = _junction_matrix(ix, iy);
          }
          junction_bucket->push_back(junction);
        }
        break;
      }
    }
    curve_bucket->push_back(dbcfg_cfg_object_location<dbcfg_curve<T>*, T>(curve, pt.x(), pt.y(), curve->min_depth(), curve->max_depth()));
    if (pt.x > _xmax) _xmax = (T) pt.x();
    if (pt.y > _ymax) _ymax = (T) pt.y();
  }
}
  
// returns a junction at the given point, or null if none exist
template <class T>
dbcfg_junction<T>* dbcfg_contour_fragment_graph<T>::find_junction(T x, T y) {
  int ix, iy;
  internal_xy(x, y, ix, iy);
  vcl_vector<dbcfg_junction<T> * >* junction_bucket = _junction_matrix(ix, iy);
  if (junction_bucket == false) {
    return false;
  }
  for (vcl_vector<dbcfg_junction<T> * >::iterator iter = junction_bucket->begin(); iter < junction_bucket->end(); iter++) {
    if ((*iter)->x() == x && (*iter)->y() == y) {
      return (*iter);
    }
  }
  return false;
}
  
// returns all curves at the given point
template <class T>
vcl_vector<dbcfg_curve<T> * > dbcfg_contour_fragment_graph<T>::find_curves(T x, T y) {
  vcl_vector<dbcfg_curve<T> * > curves;
  int ix, iy;
  internal_xy(x, y, ix, iy);
  vcl_vector<dbcfg_cfg_object_location<dbcfg_curve<T> *, T> >* curve_bucket = _curve_matrix(ix, iy);
  if (curve_bucket == false) {
    return curves;
  }
  for (vcl_vector<dbcfg_cfg_object_location<dbcfg_curve<T> *, T> >::iterator citer = curve_bucket->begin(); citer < curve_bucket->end(); citer++) {
    if ((*citer).exists_at(x, y)) {
      curves.push_back((*citer).get_object());
    }
  }
  return curves;
}
  
// returns all curves at the given point and depth
template <class T>
vcl_vector<dbcfg_curve<T> * > dbcfg_contour_fragment_graph<T>::find_curves(T x, T y, int depth) {
  vcl_vector<dbcfg_curve<T> * > curves;
  int ix, iy;
  internal_xy(x, y, ix, iy);
  vcl_vector<dbcfg_cfg_object_location<dbcfg_curve<T> *, T> >* curve_bucket = _curve_matrix(ix, iy);
  if (curve_bucket == false) {
    return curves;
  }
  for (vcl_vector<dbcfg_cfg_object_location<dbcfg_curve<T> *, T> >::iterator citer = curve_bucket->begin(); citer < curve_bucket->end(); citer++) {
    if ((*citer).exists_at(x, y, depth)) {
      curves.push_back((*citer).get_object());
    }
  }
  return curves;
}

// returns all junctions in the cfg
template <class T>
inline
vcl_vector<dbcfg_junction<T> * > dbcfg_contour_fragment_graph<T>::get_junctions() {
  return _junctions;
}

// returns all curves in the cfg
template <class T>
inline
vcl_vector<dbcfg_curve<T> * > dbcfg_contour_fragment_graph<T>::get_curves() {
  return _curves;
}

// converts the given xy to the internal matrix xy
// currently, no bounds checking - and bounds vs boundless treated the same
// this is for speed! (also note inline)
template <class T>
inline
void dbcfg_contour_fragment_graph<T>::internal_xy(T x, T y, int &ix, int &iy) {
  ix = ((int) x) % _xsize;
  iy = ((int) y) % _ysize;
}

// deletes all vcl_vectors from the internal matrices
template <class T>
void dbcfg_contour_fragment_graph<T>::clean_matrices() {
  for (int x = 0; x < _xsize; x++) {
    for (int y = 0; y < _ysize; y++) {
      if (_curve_matrix(x, y) != false) {
        delete _curve_matrix(x, y);
        _curve_matrix(x, y) = false;
      }
      if (_junction_matrix(x, y) != false) {
        delete _junction_matrix(x, y);
        _junction_matrix(x, y) = false;
      }
    }
  }
}



// -------------------------
// INSTANTIATION
// -------------------------

#define DBCFG_CONTOUR_FRAGMENT_GRAPH_INSTANTIATE(T) \
template class dbcfg_contour_fragment_graph<T>;\

#endif dbcfg_contour_fragment_graph_txx_
// end dbcfg_contour_fragment_graph.txx

