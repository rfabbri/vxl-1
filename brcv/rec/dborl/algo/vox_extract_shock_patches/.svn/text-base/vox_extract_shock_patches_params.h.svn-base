
//:
// \file
// \brief parameter set for shock patch extraction algorithm
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date June 14, 2009
//      
// \verbatim
//   Modifications
//  
// \endverbatim

// update by 
//

#ifndef vox_extract_shock_patches_params_h_
#define vox_extract_shock_patches_params_h_

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class vox_extract_shock_patches_params : public dborl_algo_params
{
public:
  //: Constructor
  vox_extract_shock_patches_params(vcl_string algo_name);

  // MEMBER VARIABLES

  //: Save result to the object folder?
  dborl_parameter<bool> save_to_object_folder_;  
  
  //: Name of input object
  dborl_parameter<vcl_string> input_object_name_;
  
  //: passes the folder of the input object
  dborl_parameter<vcl_string> input_object_dir_;    

  //: passes the folder of the input assoc directory
  dborl_parameter<vcl_string> input_assoc_dir_;    

  //: extension of the input extrinsinc shock file ( .esf ) 
  dborl_parameter<vcl_string> input_shock_extension_;     

  //: extension of the image for shock patch extraction
  dborl_parameter<vcl_string> input_image_extension_;     

  // if written to this folder as opposed to object folder then the 
  // shock patches gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_shock_patch_folder_;  

  //: tag for intrinsinc shock computation
  vcl_string tag_extract_shock_patches_;
  
};

#endif  //_vox_extract_shock_patches_params_h
