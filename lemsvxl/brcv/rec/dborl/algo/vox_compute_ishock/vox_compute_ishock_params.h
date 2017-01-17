// This is dborl/algo/vox_compute_ishock/vox_compute_ishock_params.h

//:
// \file
// \brief parameter set for compute_ishock algorithm
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date May 17, 2008
//      
// \verbatim
//   Modifications
//  
// \endverbatim

// update by Maruthi Narayanan (mn@lems.brown.edu)
//

#ifndef vox_compute_ishock_params_h_
#define vox_compute_ishock_params_h_

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class vox_compute_ishock_params : public dborl_algo_params
{
public:
  //: Constructor
  vox_compute_ishock_params(vcl_string algo_name);

  // MEMBER VARIABLES

  //: Save result to the object folder?
  dborl_parameter<bool> save_to_object_folder_;  
  
  //: Name of input object
  dborl_parameter<vcl_string> input_object_name_;
  
  //: passes the folder of the input object
  dborl_parameter<vcl_string> input_object_dir_;    

  //: passes the input association dir
  dborl_parameter<vcl_string> input_assoc_dir_;

  //: extension of the input contour file ( .cem,.cemv,.con) 
  dborl_parameter<vcl_string> input_contour_extension_;     

  //: extension of the image ( .cem,.cemv,.con) 
  dborl_parameter<vcl_string> input_image_extension_;     

  //: extension of output file
  dborl_parameter<vcl_string> output_extension_;

  // if written to this folder as opposed to object folder then the shock graph 
  // gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_shock_folder_;  

  //: Number of iterations for valid shock computation
  dborl_parameter<int> num_iter_;  

  //: Perform gap transform on intrinsinc shock graph
  dborl_parameter<bool> gap_transform_;  

  //: Add a bounding box to vsol for shock computation
  dborl_parameter<bool> add_bbox_;  

  //: tag for intrinsinc shock computation
  vcl_string tag_compute_ishock_;

  //: tag for gap transform
  vcl_string tag_gap_transform_;

  //: tag for sample shock
  vcl_string tag_sample_shock_;
  
};

#endif  //_vox_compute_ishock_params_h
