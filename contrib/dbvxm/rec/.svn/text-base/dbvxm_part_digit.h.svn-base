// This is contrib/dbvxm/rec/dbvxm_part_digit.h
#ifndef dbvxm_part_digit_h_
#define dbvxm_part_digit_h_
//:
// \file
// \brief class to represent primitive parts for digit recognition
//        The primitive parts are non-isotropic gaussian filters oriented in various ways
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
//

#include <rec/dbvxm_part_base.h>
#include <vcl_vector.h>
#include <rec/dbvxm_part_digit_sptr.h>

#include <vil/vil_image_resource_sptr.h>

class dbvxm_part_digit : public dbvxm_part_instance {

public:

  dbvxm_part_digit(float x, float y, float strength, float lambda0, float lambda1, float theta, unsigned type) : dbvxm_part_instance(0, type, x, y, strength), lambda0_(lambda0), lambda1_(lambda1), theta_(theta) {};

  //: Print an ascii summary to the stream
  virtual void print_summary(vcl_ostream &os) const
  {
    os << "x: " << x_ << " y: " << y_ << " strength: " << strength_ << vcl_endl;
    os << "lambda0: " << lambda0_ << " lambda1: " << lambda1_ << " theta: " << theta_ << vcl_endl;
  }

  virtual dbvxm_part_digit* cast_to_digit(void);

  float lambda0_;  // axis 
  float lambda1_;
  float theta_;    // orientation angle (in degrees)
  
};

//strength_threshold in [0,1] - min strength to declare the part as detected
//: extracts only one type of primitive and adds to the part vector
bool extract_digit_primitives(vil_image_resource_sptr img, float lambda0, float lambda1, float theta, bool bright, float strength_threshold, unsigned type, vcl_vector<dbvxm_part_instance_sptr>& parts);

#endif  // dbvxm_part_digit_h_
