// This is brcv/rec/dborl/algo/vox_octave_create_image_pyramid/vox_octave_create_image_pyramid_params.h

//:
// \file
// \brief parameter set
//
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Feb 24, 2010
//

#ifndef vox_octave_create_image_pyramid_params_h_
#define vox_octave_create_image_pyramid_params_h_

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them
//  to the parameter list in the constructor so that
//  all the files related to the parameter set of the algorithm
//  are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks
//  to the parameter list
class vox_octave_create_image_pyramid_params : public dborl_algo_params
{
public:

  //: Constructor
  vox_octave_create_image_pyramid_params(vcl_string algo_name);

  // MEMBER VARIABLES

  //: Name of input object
  dborl_parameter<vcl_string> input_object_name_;

  //: passes the folder of the input object
  dborl_parameter<vcl_string> input_object_dir_;

  //: extension of the input image
  dborl_parameter<vcl_string> input_extension_;

  //: extension of output file
  dborl_parameter<vcl_string> output_extension_;

  // if written to this folder as opposed to object folder then the shock graph
  // gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_pyramid_folder_;

  dborl_parameter<vcl_string>  interp_type_;

  dborl_parameter<double> step_base_;

  dborl_parameter<double> step_power_;

  dborl_parameter<int> num_steps_;

};

#endif  //_vox_octave_create_image_pyramid_params_h
