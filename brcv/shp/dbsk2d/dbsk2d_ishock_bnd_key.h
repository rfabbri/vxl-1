// This is brcv/shp/dbsk2d/dbsk2d_ishock_bnd_key.h
#ifndef dbsk2d_ishock_bnd_key_h_
#define dbsk2d_ishock_bnd_key_h_
//:
// \file
// \brief Key to the bnd ishock map
// \author Amir Tamrakar
// \date 04/18/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 04/18/2005    Initial version.
//
// \endverbatim

#include <vcl_string.h>
#include <dbsk2d/dbsk2d_ishock_elm.h>

//: This class is a key to the bnd_ishock_map
class dbsk2d_ishock_bnd_key
{
public:
  enum eta_type { ANGLE, LENGTH };
  enum shock_type 
  { 
    LEFT,         ///> boundary element(wavefront) is on the left the side of the shock
    RIGHT,        ///> boundary element(wavefront) is on the right the side of the shock 
    LEFTCONTACT,  ///> shock is the leftmost contact (for degenerate cases)
    RIGHTCONTACT, ///> shock is the rightmost contact (for degenerate cases)
    QUERY      ///> when orientation doesn't matter (for querying shock from eta)
  }; 

  double s_eta; ///> starting eta
  shock_type type; ///> orientation of shock for proper ordering
  eta_type ftype; ///> for choosing the right fuzzy function

  dbsk2d_ishock_bnd_key(double eta, shock_type Stype, eta_type ft=ANGLE):
    s_eta(eta), type(Stype), ftype(ft) {}

  ~dbsk2d_ishock_bnd_key(){}

  //: is it a shock with this bnd on the left?
  bool is_left_type() const { return type==LEFT || type==LEFTCONTACT; }

  //: is it a shock with this bnd on the right?
  bool is_right_type() const { return type==RIGHT || type==RIGHTCONTACT; }

  //: return a string describing the shock type
  vcl_string type_string() const;

  //: strict weak ordering 
  // order by smaller eta (fuzzy)
  bool operator < (const dbsk2d_ishock_bnd_key& other) const;

};

#endif // dbsk2d_ishock_bnd_key_h_
