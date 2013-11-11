#include <splr/splr_pizza_slice_symmetry.h>
#include <vnl/vnl_math.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vnl/xio/vnl_xio_quaternion.h>
#include <vnl/xio/vnl_xio_vector_fixed.h>
#include "splr_pizza_slice_symmetry_representatives.h"
#include <vcl_fstream.h>

splr_pizza_slice_symmetry::splr_pizza_slice_symmetry(vnl_quaternion<double> camera0_to_camera1)
  : camera0_to_camera1_(camera0_to_camera1){
  num_slices_ = static_cast<unsigned int>(.5 + 2*vnl_math::pi/ camera0_to_camera1_.angle());
} 

splr_symmetry_struct splr_pizza_slice_symmetry::apply(orbit_index t, unsigned int coset) const {
  //should fix this later---need to handle case where pizza slice symmetry applies to a single uniform orbit
  //compute coset * rotation
  //error could accumulate; would be better to have a quaternion method : k-fold composition 
  vnl_quaternion<double> rotation_to_coset(camera0_to_camera1_.axis(), coset * camera0_to_camera1_.angle());
  //Create transformation taking representative to representee
  vnl_double_3x3 R(rotation_to_coset.rotation_matrix_transpose ());
  vgl_h_matrix_3d<double> vgmat(R.transpose(),vnl_double_3(0.,0.,0.));
  biob_worldpt_transformation transformation(vgmat);
  orbit_index modified_orbit_index = (t + num_slices_ - coset) % num_slices_;
  return splr_symmetry_struct(modified_orbit_index, transformation);
}

unsigned int splr_pizza_slice_symmetry::size() {return num_slices_;} 
