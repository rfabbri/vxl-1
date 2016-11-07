//      dbcfg_curve.h
#ifndef dbcfg_curve_h_
#define dbcfg_curve_h_

//:
// \file
// \the representation of contour fragment graph curves and junctions
// \author Benjamin Gould
// \date 7/15/09
//
//
// \verbatim
//  Modifications
// \endverbatim

/*
++==========================================================
||  
||  :: TODO ::
||  
||  Improve memory management in curves and junctions:
||    - prevent memory leaks on destruction
||  
++==========================================================
*/

#include <vcl_vector.h>
#include <dbdet/sel/dbdet_edgel.h>

// For quick powerful access on an essentially static structure, use cyclic
// dependancy. Because of this dependancy, declare curve and junction in the
// same header.
template <class T>
class dbcfg_curve;
template <class T>
class dbcfg_junction;



// -------------------------
// help functions
// -------------------------
inline
bool share_depth(int min_depth1, int max_depth1, int min_depth2, int max_depth2) {
  return (min_depth1 >= min_depth2 && min_depth1 <= max_depth2) || (min_depth2 >= min_depth1 && min_depth2 <= max_depth1);
}


// -------------------------
// dbcfg_junction
// -------------------------

template <class T>
class dbcfg_junction {

public:
  // create an unconnected junction
  dbcfg_junction(T x, T y);

  // create a junction connected to a vector of curves
  dbcfg_junction(T x, T y, vcl_vector<dbcfg_curve<T> * > curves);

  // destruct the junction
  ~dbcfg_junction();

  // returns the x-coordinate of the junction
  T x() const;

  // returns the y-coordinate of the junction
  T y() const;

  // returns the curves in the junction
  vcl_vector<dbcfg_curve<T> * > get_curves();

  // returns the curves in the junction that exist at the given depth
  vcl_vector<dbcfg_curve<T> * > get_curves(int depth);

  // returns true if the junction exists (connects to more than one curve) at the given depth
  bool exists_at(int depth);

  // add a new curve to the connection
  // returns true if successful
  bool connect_curve(dbcfg_curve<T> * curve);

  // removes a curve from the connection
  // returns true if successful
  bool disconnect_curve(dbcfg_curve<T> * curve);

  // returns true if the junction contains this curve
  bool contains_curve(dbcfg_curve<T> * curve);

  // returns true if the junction lies on the curve
  bool can_connect(dbcfg_curve<T> * curve);

private:
  // x-coordinate of the junction
  T _x;

  // y-coordinate of the junction
  T _y;

  // stored connected curves
  vcl_vector<dbcfg_curve<T> * > _curves;
};



// -------------------------
// dbcfg_curve
// -------------------------

template <class T>
class dbcfg_curve {

public:
  // create a base curve from an edgel chain
  dbcfg_curve(dbdet_edgel_chain* chain, int depth = 1);

  // create a parent curve from other curves
  // given curves should be in order joining
  dbcfg_curve(vcl_vector<dbcfg_curve<T> * > children, int depth = 1);

  // destruct the curve
  ~dbcfg_curve();

  // returns true if the curve has no children
  bool is_base();

  // returns true if the curve has no parent
  bool is_root();

  // returns the minimum depth the curve exists at
  // remember that lower depths correspond to higher on the 'tree'
  int min_depth();

  // returns the maximum depth the curve exists at
  // remember that higher depths correspond to lower on the 'tree'
  int max_depth();

  // returns true if the curve exists at the given depth
  bool exists_at(int depth);

  // returns the edgel chain that makes up this curve
  dbdet_edgel_chain* get_edgel_chain();

  // returns the junctions that connect with this curve
  vcl_vector<dbcfg_junction<T> * > get_junctions();

  // returns the junctions that connect with this curve at the given depth
  vcl_vector<dbcfg_junction<T> * > get_junctions(int depth);

  // returns a junction of this curve at the given point, or null
  dbcfg_junction<T>* find_junction(T x, T y);

  // intersects this curve with the given junction
  bool add_junction(dbcfg_junction<T>* junction);

  // intersects this curve into the given curve, creating a junction if none exists
  // curves must be intersected at a known point
  // returns the created junction, or null if the intersecting junction already exists
  dbcfg_junction<T>* intersect(dbcfg_curve<T>* curve, T x, T y);

protected:
  // merges (averages) two intersecting edgels
  //dbdet_edgel* merge_edgels(dbdet_edgel* edgel1, dbdet_edgel* edgel2);

private:
  // parent of the curve
  dbcfg_curve<T> * _parent;

  // children of the curve
  vcl_vector<dbcfg_curve<T> * > _children;

  // junctions that this curve is part of
  vcl_vector<dbcfg_junction<T> * > _junctions;

  // edgel chain
  dbdet_edgel_chain* _chain;

  // depth level
  int _depth;

  // smallest child depth (highest depth in tree)
  int _child_depth;
};





#endif dbcfg_curve_h_
// end dbcfg_curve.h

