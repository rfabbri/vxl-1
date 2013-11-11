
//:
// \file
// \brief parameter set for evaluating the results of shock patch detection
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date July 15, 2009
//      
// \verbatim
//   Modifications
//  
// \endverbatim

// update by 
//

#ifndef vox_eval_shock_patches_params_h_
#define vox_eval_shock_patches_params_h_

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class vox_eval_shock_patches_params : public dborl_algo_params
{
public:

  //: Constructor
  vox_eval_shock_patches_params(vcl_string algo_name);

  // MEMBER VARIABLES
  
  //: Save result to the object folder?
  dborl_parameter<bool> save_to_object_folder_;  
  
  //: Name of model to compare against
  dborl_parameter<vcl_string> model_object_name_;
  
  //: Passes the folder of the model shock graph
  dborl_parameter<vcl_string> model_object_dir_;    

  //: Name of query to compare against
  dborl_parameter<vcl_string> query_object_name_;
  
  //: Passes the folder of the query object
  dborl_parameter<vcl_string> query_object_dir_;    

  //: Passes the query index filename
  dborl_parameter<vcl_string> query_index_filename_;

  //: extension of the html file of the bounding box detection results
  dborl_parameter<vcl_string> input_bbox_detect_extension_;     

  //: overlap ratio for detection
  dborl_parameter<float> bbox_overlap_ratio_;     

  //: extension of the output for output evaluation of shock patch detection
  dborl_parameter<vcl_string> output_shock_patch_eval_extension_;     

  // if written to this folder as opposed to object folder then the 
  // shock patch evaluation results gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_shock_patch_eval_folder_;  

};

#endif  //_vox_eval_shock_patches_params_h
