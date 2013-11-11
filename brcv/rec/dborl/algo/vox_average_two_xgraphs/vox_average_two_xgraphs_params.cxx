// This is rec/dborl/algo/vox_average_two_xgraphs/vox_average_two_xgraphs_params.cxx

//:
// \file



#include "vox_average_two_xgraphs_params.h"
#include <dborl/algo/dborl_algo_params.h>

//: Constructor
vox_average_two_xgraphs_params::
vox_average_two_xgraphs_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 
  //: Name of input model
  this->model_object_name_.set_values(this->param_list_, 
                 "io", 
                 "model_object_name",
                 "model_object_name", 
                 "dummy", 
                 "dog1",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input model
  this->model_object_dir_.set_values(this->param_list_, 
                 "io", 
                 "model_object_dir",
                 "model object folder", 
                 "dummy", 
                 "V:/projects/kimia/shockshape/learnskeleton/data/vox-kimia-99-dataset",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);

  //: Name of input query
  this->query_object_name_.set_values(this->param_list_, 
                 "io", 
                 "query_object_name",
                 "query_object_name", 
                 "dummy", 
                 "dog3",
                 1, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input query
  this->query_object_dir_.set_values(this->param_list_, 
                 "io", 
                 "query_object_dir",
                 "query object folder", 
                 "", 
                 "V:/projects/kimia/shockshape/learnskeleton/data/vox-kimia-99-dataset",
                 1, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);
 
  // Output shock folder for matches
  this->output_average_xgraph_folder_.set_values(this->param_list_, 
                 "io", 
                 "output_average_xgraph_folder", 
                 "output folder to write average xgraph results", 
                 "", // value
                 "D:/vision/docs/learnskeleton-docs/vox-output", // default value
                 1, // associated to 1st input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "average_xgraph_file",
                 dborl_parameter_type_info::FILEASSOC);

  // Process parameters
  this->convert_sk2d_to_sksp_tol_.set_values(this->param_list_, 
    "io", "convert_sk2d_to_sksp_tol", 
    "[CONVERT] Tolerance for converting sk2d graph to sksp graph (pixels)", 
    0.5f,
    0.5f);

  // Parameters for averaging two xgraphs
  this->average_xgraph_weight1_.set_values(this->param_list_, 
    "io", "average_xgraph_weight1", 
    "[AVG] Weight of xgraph1", 
    1.0f,
    1.0f);

  // Parameters for averaging two xgraphs
  this->average_xgraph_weight2_.set_values(this->param_list_, 
    "io", "average_xgraph_weight2", 
    "[AVG] Weight of xgraph2", 
    1.0f,
    1.0f);

  // Parameters for averaging two xgraphs
  this->average_xgraph_scurve_matching_R_.set_values(this->param_list_, 
    "io", "average_xgraph_scurve_matching_R_", 
    "[AVG] scurve_matching_R", 
    6.0f,
    6.0f);

  // Parameters for averaging two xgraphs
  this->average_xgraph_scurve_sample_ds_.set_values(this->param_list_, 
    "io", "average_xgraph_scurve_sample_ds_", 
    "[AVG] scurve_sample_ds", 
    3.0f,
    3.0f);
  
}

