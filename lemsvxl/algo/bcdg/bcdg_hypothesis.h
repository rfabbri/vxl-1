// This is algo/bcdg/bcdg_hypothesis.h
#ifndef bcdg_hypothesis_h_
#define bcdg_hypothesis_h_

//:
// \file
// \brief  A 3D explanation for a set of 2D curves (one in each frame)
// \author scates and dapachec
// \date   7/26/2004

#include <vbl/vbl_ref_count.h>
#include "bcdg_hypothesis_sptr.h"
#include "bcdg_hypothel.h"

#include <vcl_iostream.h> // temp

//: A 3D explanation for a set of 2D curves (one in each frame)
class bcdg_hypothesis : public vbl_ref_count {
public:
  bcdg_hypothesis() : _first(NULL) {}
  //: Add a hypothel to the front of the chain
  void prepend_hypothesis(bcdg_hypothel_sptr h) {
//    vcl_cerr << "Prepending hypo!\n";
    h->set_next(_first);
    _first = h;
    // Cost is basically an average of the cost of each hypothel.
    _cost = (_cost * _hypothels++ + (h->cost())) / _hypothels;
  }
  //: Return a new hypothesis exactly like this one.
  bcdg_hypothesis_sptr clone() const {
    return bcdg_hypothesis_sptr( new bcdg_hypothesis(*this) );
  }
  //: Return a hypothel
  bcdg_hypothel_sptr hypothel() const { return _first; }
  //: Return the cost of this hypothel
  double cost() const { return _cost; }
protected:
  //: The first hypothel in the list
  bcdg_hypothel_sptr _first;
  //: Total hypothels
  int _hypothels;
  //: Cost
  double _cost;
};


#endif // bcdg_hypothesis_h_
