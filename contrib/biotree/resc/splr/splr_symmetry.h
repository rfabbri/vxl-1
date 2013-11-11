#if !defined(SPLR_SYMMETRY_H_)
#define SPLR_SYMMETRY_H_

#include <biob/biob_worldpt_transformation.h>
#include <xscan/xscan_scan.h>
#include <splr/splr_populatable_worldpt_roster.h>
#include <biob/biob_worldpt_roster_sptr.h>
#include <vbl/vbl_ref_count.h>

/* 
   A symmetry provides a method that, given an orbit_index and a coset index,
   returns the geometric transformation that takes the representative to
   the given point, and the corresponding orbit_index
*/

struct splr_symmetry_struct {
   orbit_index representative_orbit_index_;
   biob_worldpt_transformation from_representative_;
   splr_symmetry_struct(orbit_index t, biob_worldpt_transformation transformation)
    :  representative_orbit_index_(t), from_representative_(transformation) {}
};

class splr_symmetry {
  public:
  virtual ~splr_symmetry();
    virtual vcl_string class_id() = 0;
    virtual unsigned int size() = 0;
    virtual splr_symmetry_struct apply(orbit_index t, unsigned int coset) const = 0;

};

#endif
