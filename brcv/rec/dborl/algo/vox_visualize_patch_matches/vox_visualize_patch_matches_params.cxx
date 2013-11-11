//:
// \file



#include "vox_visualize_patch_matches_params.h"
#include <dborl/algo/dborl_algo_params.h>
//#include <dborl/algo/dborl_utilities.h>


//: Constructor
vox_visualize_patch_matches_params::
vox_visualize_patch_matches_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 

  // Save result to the object folder?
  this->save_to_object_folder_.set_values(this->param_list_, "io", 
    "save_to_object_folder", "-io: save result to object folder ?", 
                                          false, false);

  //: Name of input model
  this->model_object_name_.
      set_values(this->param_list_, 
                 "io", "model_object_name",
                 "model_object_name", "dummy", "dymmy",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input model
  this->model_object_dir_.
      set_values(this->param_list_, 
                 "io", "model_object_dir",
                 "model object folder", "", 
                 "/vision/images/misc/object",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);

  //: Name of input query
  this->query_object_name_.
      set_values(this->param_list_, 
                 "io", "query_object_name",
                 "query_object_name", "dummy", "dymmy",
                 1, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input query
  this->query_object_dir_.
      set_values(this->param_list_, 
                 "io", "query_object_dir",
                 "query object folder", "", 
                 "/vision/images/misc/object",
                 1, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);

  //: passes the input association directory
  this->query_assoc_dir_.set_values(param_list_, 
                                     "io", "query_assoc_dir", 
                                     "path of the assoc filename", "", "", 
                                     1, // for 1th input object
                                     dborl_parameter_system_info::NOT_DEFINED, 
                                     "shock_patch_match_file", 
                                     dborl_parameter_type_info::FILEASSOC);

  //: extension of the input shock_patch extension
  this->input_shock_patch_extension_.set_values
      (this->param_list_, 
       "io", 
       "input_shock_patch_extension", 
       "-io: input shock patch extension ", 
       "_shock_patches", "_shock_patches");

  //: where to write images for the html web page
  this->
      image_write_folder_.set_values
      (this->param_list_, 
       "io", 
       "image_write_folder", 
       "-io: folder to write images for html file? ", 
 "/vision/projects/kimia/categorization/vox_shock_patch_image_writeout/images", 
 "/vision/projects/kimia/categorization/vox_shock_patch_image_writeout/images");

  //: extension of the input image
  this->input_image_extension_.set_values
      (this->param_list_, 
       "io", 
       "input_image_extension", 
       "-io: input image extension ", 
       ".png", ".png");

  //: extension of the html images 
  this->html_image_extension_.set_values
      (this->param_list_, 
       "io", 
       "html_image_extension", 
       "-io: html image extension ", 
       ".png", ".png");

  // Show html matches
  this->show_html_matches_.set_values(this->param_list_, "io", 
    "show_html_matches", "-io: should html file show shock graph matches ?", 
                                          false, false);

  // write model images only
  this->write_model_images_only_.set_values(this->param_list_, "io", 
    "write_model_images_only", "-io: write model images only ?", 
                                          false, false);

  // Output shock folder for matches
  this->output_visualize_folder_.
      set_values(this->param_list_, "io", 
                 "output_shock_match_folder", 
                 "output folder to write shock matching results", "",
                 "/vision/projects/kimia/categorization/output",
                 1, // associated to 0th input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "shock_patch_match_table",
                 dborl_parameter_type_info::FILEASSOC);


}

