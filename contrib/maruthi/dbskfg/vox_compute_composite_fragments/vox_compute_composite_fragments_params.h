
//:
// \file
// \brief parameter set for computation of composite fragments 
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date October 25th, 2010
//      
// \verbatim
//   Modifications
//  
// \endverbatim

// update by 
//

#ifndef vox_compute_composite_fragments_params_h_
#define vox_compute_composite_fragments_params_h_

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class vox_compute_composite_fragments_params : public dborl_algo_params
{
public:
  //: Constructor
  vox_compute_composite_fragments_params(vcl_string algo_name);

  // MEMBER VARIABLES

  //: Save result to the object folder?
  dborl_parameter<bool> save_to_object_folder_;  
  
  //: Name of input object
  dborl_parameter<vcl_string> input_object_name_;
  
  //: passes the folder of the input object
  dborl_parameter<vcl_string> input_object_dir_;    

  //: passes the folder of the input assoc directory
  dborl_parameter<vcl_string> input_assoc_dir_;    

  //: extension of the input cem file ( .cem ) 
  dborl_parameter<vcl_string> input_contour_extension_;     

  //: extension of the image for fragment extraction
  dborl_parameter<vcl_string> input_image_extension_;     

  // if written to this folder as opposed to object folder then the 
  // composite fragments gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_cgraph_fragments_folder_;  

  //: String for training data
  dborl_parameter<vcl_string> training_path_;

  //: String for training data
  dborl_parameter<vcl_string> texton_path_;

  //: String for training extension
  dborl_parameter<vcl_string> training_extension_;

  //: Euler Spiral Completion offset 
  dborl_parameter<double> ess_completion_;

  //: First Coefficient of logistic function 
  dborl_parameter<double> logistic_beta0_;

  //: Second Coefficient of logistic function  
  dborl_parameter<double> logistic_beta1_;

  //: tag for extraction of composite fragments
  vcl_string tag_compute_composite_fragments_;
  
};

#endif  //_vox_compute_composite_fragments_params_h
