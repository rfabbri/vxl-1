//      dbcfg_contour_fragment_graph.h
#ifndef dbcfg_contour_fragment_graph_h_
#define dbcfg_contour_fragment_graph_h_

//:
// \file
// \the representation of a hierarchical contour fragment graph
// \author Benjamin Gould
// \date 7/17/09
//
//
// \verbatim
//  Modifications
// \endverbatim

/*========================================================*\
||  
||            :: TODO ::
//

\\
||  Add epsilon factor functionality
||    - 'epsilon' currently passed to constructor but unused
||    - where are junctions placed?
||    - can we match intersections in the edgel gaps?
//

\\
||  Improve memory management in curves and junctions:
||    - prevent memory leaks on destruction
||    - manage edgel ownership
//

\\
||  Implement 'construction' via edgels
||    - prevent memory leaks on destruction
||    - manage edgel ownership
//

\\
||  Conversion to/from Eli's and Amir's
//

\\
\*========================================================*/

#include "dbcfg_curve.h"
#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <dbdet/sel/dbdet_edgel.h>

// Use a locator class for easier acces
template <class X, class T>
class dbcfg_cfg_object_location;



// -------------------------
// dbcfg_contour_fragment_graph
// -------------------------

template <class T>
class dbcfg_contour_fragment_graph {

public:
  // creates an empty contour fragment graph with the given bounds
  // this is more efficient than the boundless contructor
  dbcfg_contour_fragment_graph(int xsize, int ysize, T epsilon = 0);

  // creates an empty contour fragment graph with no bounds
  dbcfg_contour_fragment_graph(T epsilon = 0);

  // destruct the contour fragment graph
  ~dbcfg_contour_fragment_graph();

  // returns the maximum x value (bound) or 0 if boundless
  // [use 0 instead of -1 to allows for T being unsigned]
  T xmax();

  // returns the maximum y value (bound) or 0 if boundless
  // [use 0 instead of -1 to allows for T being unsigned]
  T ymax();

  // returns the maximum x value (bound) or the greatest x value if boundless
  T width();

  // returns the maximum y value (bound) or the greatest y value if boundless
  T height();

  // resets the cfg
  void reset(int xsize, int ysize);

  // add a new curve to the cfg, populating junctions as needed
  // because higher level curves are built on lower level ones, curves should be
  // created from the bottom up (highest depth to lowest depth)
  void add_curve(dbcfg_curve<T>* curve);
  
  // returns a junction at the given point, or null if none exist
  dbcfg_junction<T>* find_junction(T x, T y);
  
  // returns all curves at the given point
  vcl_vector<dbcfg_curve<T> * > find_curves(T x, T y);
  
  // returns all curves at the given point and depth
  vcl_vector<dbcfg_curve<T> * > find_curves(T x, T y, int depth);

  // returns all junctions in the cfg
  vcl_vector<dbcfg_junction<T> * > get_junctions();

  // returns all curves in the cfg
  vcl_vector<dbcfg_curve<T> * > get_curves();

protected:
  // converts the given xy to the internal matrix xy
  void internal_xy(T x, T y, int &ix, int &iy);

  // deletes all vcl_vectors from the internal matrices
  void clean_matrices();

private:
  // the internal 'bucket' matrix for curves
  // note that curves only exist on a range, and are hard to location
  // for these reasons, an object_location is used
  vnl_matrix<vcl_vector<dbcfg_cfg_object_location<dbcfg_curve<T> *, T> > * > _curve_matrix;

  // the internal 'bucket' matrix for junctions
  vnl_matrix<vcl_vector<dbcfg_junction<T> * > * > _junction_matrix;

  // all curves
  vcl_vector<dbcfg_curve<T> * > _curves;

  // all junctions
  vcl_vector<dbcfg_junction<T> * > _junctions;

  // boundless marker
  bool _boundless;

  // bounds
  int _xsize;
  int _ysize;

  // max value
  T _xmax;
  T _ymax;

  // epsilon factor
  T _epsilon;
};



// -------------------------
// dbcfg_cfg_object_location
// -------------------------

template <class X, class T>
class dbcfg_cfg_object_location {

public:
  // create an object location with x, y, and optionally, depths
  dbcfg_cfg_object_location(X object, T x, T y, int min_depth = 1, int max_depth = 1) :
      _object(object),
      _x(x),
      _y(y),
      _min_depth(min_depth),
      _max_depth(max_depth) {
  }

  // returns the object
  X get_object() {
    return _object;
  }

  // returns true if the given location matches with this one
  bool exists_at(T x, T y) {
    return x == _x && y == _y;
  }

  // returns true if the given location matches with this one
  bool exists_at(T x, T y, int depth) {
    return x == _x && y == _y && depth <= _max_depth && depth >= _min_depth;
  }

  // returns true if the given location matches with this one
  bool exists_at(T x, T y, int min_depth, int max_depth) {
    return x == _x && y == _y && share_depth(min_depth, max_depth, _min_depth, _max_depth);
  }

private:
  // object at location
  X _object;

  // location
  T _x;
  T _y;

  // depth
  int _min_depth;
  int _max_depth;
};



#endif dbcfg_contour_fragment_graph_h_
// end dbcfg_contour_fragment_graph.h

