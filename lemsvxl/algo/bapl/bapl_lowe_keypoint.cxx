// This is algo/bapl/bapl_lowe_keypoint.cxx
//:
// \file

#include "bapl_lowe_keypoint.h"
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <bapl/bapl_lowe_pyramid_set.h>

//: Constructor
bapl_lowe_keypoint::bapl_lowe_keypoint()
{
  // make a random keypoint
  for( int i=0; i<128; ++i){
    descriptor_[i] = vcl_rand();
  }
  this->normalize();
}

//: Constructor
bapl_lowe_keypoint::bapl_lowe_keypoint( bapl_lowe_pyramid_set_sptr pyramid_set,
                                        double i, double j, double s, double o )
 : pyramid_set_(pyramid_set), location_i_(i),
   location_j_(j), scale_(s), orientation_(o)
{
  pyramid_set_->make_descriptor(this);
  this->normalize();
}

//: Destructor
bapl_lowe_keypoint::~bapl_lowe_keypoint()
{
  // This destructor does absolutely nothing
}

//: Accessor for the discriptor vector
const vnl_vector_fixed<double,128>&
bapl_lowe_keypoint::descriptor() const
{
  return descriptor_;
}


//: Mutator for the discriptor vector
void
bapl_lowe_keypoint::set_descriptor(const vnl_vector_fixed<double,128>& descriptor)
{
  descriptor_ = descriptor;
  this->normalize();
}


//: Normalize, threshold, and renormalize
void
bapl_lowe_keypoint::normalize()
{
  descriptor_.normalize();
  bool changed = false;
  for( int i=0; i<128; ++i){
    if( descriptor_[i] > 0.2 ){
      descriptor_[i] = 0.2;
      changed = true;
    }
  }
  
  if( changed )
    descriptor_.normalize();
}


//: Print a summary of the keypoint data to a stream
void
bapl_lowe_keypoint::print_summary(vcl_ostream& os) const
{
  os << "Lowe keypoint: ("<<location_i_<<','<<location_j_<<") scale="<<scale_<<" orientation="<<orientation_<<vcl_endl;
  //os << "               desc="<< descriptor_ << vcl_endl;
}
