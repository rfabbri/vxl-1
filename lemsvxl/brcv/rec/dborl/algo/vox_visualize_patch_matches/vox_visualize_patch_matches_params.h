
//:
// \file
// \brief parameter set for visualize shock patches
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date July 11, 2009
//      
// \verbatim
//   Modifications
//  
// \endverbatim

// update by 
//

#ifndef vox_visualize_patch_matches_params_h_
#define vox_visualize_patch_matches_params_h_

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class vox_visualize_patch_matches_params : public dborl_algo_params
{
public:
  //: Constructor
  vox_visualize_patch_matches_params(vcl_string algo_name);

  // MEMBER VARIABLES
 
  //: Save result to the object folder?
  dborl_parameter<bool> save_to_object_folder_;  

  //: Name of model to compare against
  dborl_parameter<vcl_string> model_object_name_;
  
  //: Passes the folder of the model shock graph
  dborl_parameter<vcl_string> model_object_dir_;    

  //: Name of query to compare against
  dborl_parameter<vcl_string> query_object_name_;
  
  //: Passes the folder of the query shock graph
  dborl_parameter<vcl_string> query_object_dir_;    

  //: Passes the folder of where the match objects can live
  dborl_parameter<vcl_string> query_assoc_dir_;

  //: extension of the folder where shock patches can be found
  dborl_parameter<vcl_string> input_shock_patch_extension_;     

  //: Passes the folder where to write images for html file
  dborl_parameter<vcl_string> image_write_folder_;    

  //: extension of the input image image file, same for both model and query 
  dborl_parameter<vcl_string> input_image_extension_;     

  //: extension of the image file that html will address
  dborl_parameter<vcl_string> html_image_extension_;     
  
  //: show matching shock graphs in html file
  dborl_parameter<bool> show_html_matches_;  

  //: write model images only
  dborl_parameter<bool> write_model_images_only_;  

  // if written to this folder as opposed to object folder then the 
  // shock matches gets associated to the input object.
  // if nothing is written here, nothing gets associated
  dborl_parameter<vcl_string> output_visualize_folder_;  

};

#endif  //_vox_visualize_patch_matches_params_h
