#if !defined(SPLR_PIZZA_SLICE_SYMMETRY_H_)
#define SPLR_PIZZA_SLICE_SYMMETRY_H_

#include "splr_symmetry.h"
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <biob/biob_explicit_worldpt_roster_sptr.h>
#include <xscan/xscan_uniform_orbit.h>
#include <vnl/vnl_quaternion.h>
#include <vcl_iostream.h>

class splr_pizza_slice_symmetry : public splr_symmetry {
 private:
   unsigned int num_slices_;
   vnl_quaternion<double> camera0_to_camera1_;
 public:
  splr_pizza_slice_symmetry(vnl_quaternion<double> camera0_to_camera1);
  splr_symmetry_struct apply(orbit_index t, unsigned int coset) const;
  vcl_string class_id() { return "splr_pizza_slice_symmetry"; }
  //: number of cosets
  unsigned int size();
  //  vnl_quaternion<double> camera0_to_camera1() { return camera0_to_camera1_; }
  virtual ~splr_pizza_slice_symmetry(){};
};


//void x_write(vcl_ostream &os, splr_pizza_slice_symmetry sym);
#endif
