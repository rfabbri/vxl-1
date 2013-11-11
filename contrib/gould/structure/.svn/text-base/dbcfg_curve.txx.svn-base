//      dbcfg_curve.txx
#ifndef dbcfg_curve_txx_
#define dbcfg_curve_txx_

// Template for contour fragment graph's curve
// Benjamin Gould
// 7/16/09

#include "dbcfg_curve.h"
#include <vcl_deque.h>
#include <vgl/vgl_point_2d.h>

// create a base curve from an edgel chain
template <class T>
dbcfg_curve<T>::dbcfg_curve(dbdet_edgel_chain* chain, int depth) :
_parent(false),
_chain(chain),
_depth(depth),
_child_depth(INT_MAX) {
}

// create a parent curve from other curves
// given curves should be in order joining
template <class T>
dbcfg_curve<T>::dbcfg_curve(vcl_vector<dbcfg_curve<T> * > children, int depth) :
_parent(false),
_children(children),
_depth(depth) {
  vcl_deque<dbdet_edgel * > edgels;
  for (vcl_vector<dbcfg_curve<T> * >::iterator iter = children.begin(); iter < children.end(); iter++) {
    vcl_deque<dbdet_edgel * > newedgels = (*iter)->get_edgel_chain()->edgels;
    bool reverse_edgels = false;
    bool reverse_newedgels = false;
    if (!edgels.empty()) {
      dbdet_edgel* edgel1a = &*(edgels.front());
      dbdet_edgel* edgel1b = &*(edgels.back());
      dbdet_edgel* edgel2a = &*(newedgels.front());
      dbdet_edgel* edgel2b = &*(newedgels.back());
      //   a------------  &  a------------
      if (edgel1a->pt.x() == edgel2a->pt.x() && edgel1a->pt.y() == edgel2a->pt.y()) {
        //edgels.pop_front();
        //newedgels.pop_front();
        reverse_edgels = true;
        //edgels.push_back(merge_edgels(edgel1a, edgel2a));
      //   a------------  &  ------------b
      } else if (edgel1a->pt.x() == edgel2b->pt.x() && edgel1a->pt.y() == edgel2b->pt.y()) {
        //edgels.pop_front();
        //newedgels.pop_back();
        reverse_edgels = true;
        reverse_newedgels = true;
        //edgels.push_back(merge_edgels(edgel1a, edgel2b));
      //   ------------b  &  a------------
      } else if (edgel1b->pt.x() == edgel2a->pt.x() && edgel1b->pt.y() == edgel2a->pt.y()) {
        //edgels.pop_back();
        //newedgels.pop_front();
        //edgels.push_back(merge_edgels(edgel1b, edgel2a));
      //   ------------b  &  ------------b
      } else if (edgel1b->pt.x() == edgel2b->pt.x() && edgel1b->pt.y() == edgel2b->pt.y()) {
        //edgels.pop_back();
        //newedgels.pop_back();
        reverse_newedgels = true;
        //edgels.push_back(merge_edgels(edgel1b, edgel2b));
      } else {
        continue;
      }
    }

    vcl_deque<dbdet_edgel * >::iterator eiter;
    if (reverse_newedgels) eiter = newedgels.end() - 1;
    else eiter = newedgels.begin();
    while ((!reverse_newedgels && eiter < newedgels.end()) || (reverse_newedgels && eiter >= newedgels.begin())) {
      if (reverse_edgels) edgels.push_front(*eiter);
      else edgels.push_back(*eiter);
      if (reverse_newedgels) eiter--;
      else eiter++;
    }
    (*iter)->_parent = this;
  }
  _chain->edgels = edgels;

  _child_depth = INT_MAX;
  for (vcl_vector<dbcfg_curve<T> * >::iterator iter = _children.begin(); iter < _children.end(); iter++) {
    if ((*iter)->_depth < _child_depth) {
      _child_depth = (*iter)->_depth;
    }
  }
}

// destruct the curve
template <class T>
dbcfg_curve<T>::~dbcfg_curve() {
}

// returns true if the curve has no children
template <class T>
inline
bool dbcfg_curve<T>::is_base() {
  return _children.empty();
}

// returns true if the curve has no parent
template <class T>
inline
bool dbcfg_curve<T>::is_root() {
  return _parent == false;
}

// returns the minimum depth the curve exists at
// remember that lower depths correspond to higher on the 'tree'
template <class T>
inline
int dbcfg_curve<T>::min_depth() {
  return _depth;
}

// returns the maximum depth the curve exists at
// remember that higher depths correspond to lower on the 'tree'
template <class T>
inline
int dbcfg_curve<T>::max_depth() {
  return _child_depth - 1;
}


// returns true if the curve exists at the given depth
template <class T>
inline
bool dbcfg_curve<T>::exists_at(int depth) {
  return depth >= this->min_depth() && depth <= this->max_depth();
}

// returns the edgel chain that makes up this curve
template <class T>
inline
dbdet_edgel_chain* dbcfg_curve<T>::get_edgel_chain() {
  return _chain;
}

// returns the junctions that connect with this curve
template <class T>
inline
vcl_vector<dbcfg_junction<T> * > dbcfg_curve<T>::get_junctions() {
  return _junctions;
}

// returns the junctions that connect with this curve at the given depth
template <class T>
vcl_vector<dbcfg_junction<T> * > dbcfg_curve<T>::get_junctions(int depth) {
  vcl_vector<dbcfg_junction<T> * > junctions;
  if (!this->exists_at(depth)) return junctions;
  for (vcl_vector<dbcfg_junction<T> * >::iterator iter = _junctions.begin(); iter < _junctions.end(); iter++) {
    if ((*iter)->exists_at(depth)) {
      junctions.push_back(*iter);
    }
  }
  return junctions;
}

// returns a junction of this curve at the given point, or null
template <class T>
dbcfg_junction<T>* dbcfg_curve<T>::find_junction(T x, T y) {
  vcl_vector<dbcfg_junction<T> * > junctions = this->get_junctions();
  for (vcl_vector<dbcfg_junction<T> * >::iterator iter = junctions.begin(); iter < junctions.end(); iter++) {
    dbcfg_junction<T>* junction = (*iter);
    if (junction->x() == x && junction->y() == y) {
      return junction;
    }
  }
  return false;
}

// intersects this curve with the given junction
template <class T>
bool dbcfg_curve<T>::add_junction(dbcfg_junction<T>* junction) {
  if (junction->connect_curve(this)) {
    _junctions.push_back(junction);
    return true;
  }
  return false;
}

// intersects this curve into the given curve, creating a junction if none exists
// curves must be intersected at a known point
// returns the created junction, or null if the intersecting junction already exists
template <class T>
dbcfg_junction<T>* dbcfg_curve<T>::intersect(dbcfg_curve<T>* curve, T x, T y) {
  dbcfg_junction<T>* junction = false;

  // check depths
  if (!share_depth(this->min_depth(), this->max_depth(), curve->min_depth(), curve->max_depth())) {
    return false;
  }

  // find junction
  junction = curve->find_junction(x, y);
  if (junction != false) {
    this->add_junction(junction);
    return false;
  }
  junction = this->find_junction(x, y);
  if (junction != false) {
    curve->add_junction(junction);
    return false;
  }
    
  // new junction  
  junction = new dbcfg_junction<T>(x, y);
  if (!junction->can_connect(curve) || !this->add_junction(junction) || !curve->add_junction(junction)) {
    delete junction;
    return false;
  }
  return junction;
}

// merges (averages) two intersecting edgels
/*
template <class T>
dbdet_edgel* dbcfg_curve<T>::merge_edgels(dbdet_edgel* edgel1, dbdet_edgel* edgel2) {
  double tangent = (edgel1->tangent + edgel2->tangent) / 2;
  double strength = (edgel1->strength + edgel2->strength) / 2;
  double deriv = (edgel1->deriv + edgel2->deriv) / 2;
  dbdet_edgel* edgel = new dbdet_edgel(edgel1->pt, tangent, strength, deriv);
  edgel->id = edgel1->id;
  return edgel;
}
*/


// -------------------------
// INSTANTIATION
// -------------------------

#define DBCFG_CURVE_INSTANTIATE(T) \
template class dbcfg_curve<T>;\

#endif dbcfg_curve_txx_
// end dbcfg_curve.txx

