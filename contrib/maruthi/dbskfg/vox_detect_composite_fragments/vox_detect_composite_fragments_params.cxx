//:
// \file



#include "vox_detect_composite_fragments_params.h"

//: Constructor
vox_detect_composite_fragments_params::
vox_detect_composite_fragments_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 

  // Save result to the object folder?
  this->save_to_object_folder_.set_values(this->param_list_, "io", 
    "save_to_object_folder", "-io: save result to object folder ?", 
                                          false, false);

  //: Name of input query
  this->query_object_name_.
      set_values(this->param_list_, 
                 "io", "query_object_name",
                 "query_object_name", "dummy", "dymmy",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input query
  this->query_object_dir_.
      set_values(this->param_list_, 
                 "io", "query_object_dir",
                 "query object folder", "", 
                 "/vision/images/misc/object",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);

  // Extension for input association file
  this->input_assoc_dir_.set_values(param_list_, 
                                     "io", "input_assoc_dir", 
                                     "path of the assoc filename", "", "", 
                                     0, // for 0th input object
                                     dborl_parameter_system_info::NOT_DEFINED, 
                                     "cgraph_fragment_match_file", 
                                     dborl_parameter_type_info::FILEASSOC);

  //: extension of the input cgraph fragment match file extension
  this->input_cgraph_match_file_extension_.set_values
      (this->param_list_, 
       "io", 
       "input_cgraph_match_file_extension", 
       "-io: input composite graph matching file extension ", 
       "_match_similarity_matrix.xml", "_match_similarity_matrix.xml");


  //: Minimum threshold for cgraph matching costs
  this->min_threshold_.set_values(this->param_list_, 
                                  "detect_params", 
                                  "min_threshold", 
                                  "[detect-params] min threshold", 
                                  0.1f, 0.1f);

  //: Maximum threshold for cgraph matching costs
  this->max_threshold_.set_values(this->param_list_, 
                                  "detect_params", 
                                  "max_threshold", 
                                  "[detect-params] max threshold", 
                                  1.0f, 1.0f);

  //: Threshold increment for cgraph matching costs
  this->threshold_inc_.set_values(this->param_list_, 
                                  "detect_params", 
                                  "threshold_inc", 
                                  "[detect-params] threshold increment", 
                                  0.1f, 0.1f);

  //: Number of model N patches that must match at minimum 
  this->model_N_.set_values
      (this->param_list_, 
       "detect_params", 
       "model_N", 
       "[detect-params] use top N query patches for each model patch", 
       3, 3);

  //: Number of k model patches that must match at minimum 
  this->top_k_.set_values
      (this->param_list_, 
       "detect_params", 
       "top_k", 
       "[detect-params] k model patches have at least N query patches", 
       2, 2);

  // Output cgraph matching folder (if not object folder)
  this->output_cgraph_fragment_detect_folder_.
      set_values(this->param_list_, "io", 
                 "output_cgraph_fragment_detect_folder", 
                 "output folder to write the bbox from cgraph detection",
                 "",
                 "/vision/projects/kimia/categorization/output",
                 0, // associated to 0th input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "shock_patch_detection",
                 dborl_parameter_type_info::FILEASSOC);


 
}

