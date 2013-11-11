//:
// \file



#include "vox_eval_shock_patches_params.h"
#include <dborl/algo/dborl_algo_params.h>

//: Constructor
vox_eval_shock_patches_params::
vox_eval_shock_patches_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 

  // Save result to the object folder?
  this->save_to_object_folder_.set_values(this->param_list_, "io", 
    "save_to_object_folder", "-io: save result to object folder ?", 
                                          true, true);

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
                 "query object name", "dummy", "dymmy",
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

  // Index file for query dataset
  this->query_index_filename_.set_values(
      param_list_, 
      "io", "query_index_filename", 
      "path of the index file", "", "", 
      1, 
      dborl_parameter_system_info::NOT_DEFINED, 
      "flat_image", 
      dborl_parameter_type_info::FILEASSOC);

  //: extension of the html file of the bounding box detect results
  this->input_bbox_detect_extension_.set_values
      (this->param_list_, 
       "io", 
       "input_bbox_detect_extension", 
       "-io: input extension of bbox detection results ", 
       "_detect_results.xml", "_detect_results.xml");

  //: extension of the html file of the bounding box detect results
  this->bbox_overlap_ratio_.set_values
      (this->param_list_, 
       "io", 
       "bbox_overlap_ratio", 
       "-io: amount of bbox overlap with groundtruth to consider detection ? ", 
       .50, .50);

  //: output extension of the evaluation results to write to
  this->output_shock_patch_eval_extension_.set_values
      (this->param_list_, 
       "io", 
       "output_shock_patch_eval_extension", 
       "-io: output extension of shock patch evaluation results on dataset ", 
       "_dataset_eval_results.xml", "_dataset_eval_results.xml");

  // Output shock folder for evaluation of detect results
  this->output_shock_patch_eval_folder_.
      set_values(this->param_list_, "io", 
                 "output_shock_patch_eval_folder", 
                 "output folder to write evaluation results off experiment", "",
                 "/vision/projects/kimia/categorization/output",
                 0, // associated to 1th input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "shock_patch_eval",
                 dborl_parameter_type_info::FILEASSOC);

}

