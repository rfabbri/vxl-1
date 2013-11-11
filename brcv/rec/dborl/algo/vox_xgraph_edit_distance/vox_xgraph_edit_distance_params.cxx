// This is rec/dborl/algo/vox_average_two_xgraphs/vox_xgraph_edit_distance_params.cxx

//:
// /file



#include "vox_xgraph_edit_distance_params.h"
#include <dborl/algo/dborl_algo_params.h>

//: Constructor
vox_xgraph_edit_distance_params::
vox_xgraph_edit_distance_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 
  //: Name of input model
  this->model_dataset_name_.set_values(this->param_list_, 
                 "io", 
                 "model_dataset_name",
                 "model_dataset_name", 
                 "dummy", 
                 "kimia_99_dog",
                 1, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input model
  this->model_dataset_dir_.set_values(this->param_list_, 
                 "io", 
                 "model_dataset_dir",
                 "model_dataset_dir", 
                 "dummy", 
                 "V:/projects/kimia/shockshape/learnskeleton/data/vox-kimia-99-dataset",
                 1, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);

  // Index file for list of objects in dataset
  this->model_dataset_index_file_.set_values(param_list_, 
                                "io", 
                                "index_filename", 
                                "path of the index file", 
                                "dummy", 
                                "V:/projects/kimia/shockshape/learnskeleton/data/index_kimia_99_3_dogs.xml", 
                                1, 
                                dborl_parameter_system_info::NOT_DEFINED,
                                "flat_image", 
                                dborl_parameter_type_info::FILEASSOC);


  //: Name of input query
  this->query_object_name_.set_values(this->param_list_, 
                 "io", 
                 "query_object_name",
                 "query_object_name", 
                 "dummy", 
                 "dog3",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input query
  this->query_object_dir_.set_values(this->param_list_, 
                 "io", 
                 "query_object_dir",
                 "query object folder", 
                 "", 
                 "V:/projects/kimia/shockshape/learnskeleton/data/vox-kimia-99-dataset",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);


  
  // flag to use existing xgraph file
  this->use_existing_xgraph_file_.set_values(this->param_list_, 
    "io", "use_existing_xgraph_file", 
    "[INPUT] Use existing xgraph file when it is available", 
    true,
    true);


    // file extension of model xgraph
  this->model_xgraph_extension_.set_values(this->param_list_, 
    "io", "model_xgraph_extension", 
    "[INPUT] File extension of existing model xgraph", 
    "-xgraph.xml",
    "-xgraph.xml");

  // file extension of query xgraph
  this->query_xgraph_extension_.set_values(this->param_list_, 
    "io", "query_xgraph_extension", 
    "[INPUT] File extension of existing query xgraph", 
    "-xgraph.xml",
    "-xgraph.xml");


 
  // Output shock folder for matches
  this->output_folder_.set_values(this->param_list_, 
                 "io", 
                 "output_folder", 
                 "output folder to write edit distance results", 
                 "xgraph_edit_distance", // value
                 "D:/vision/docs/learnskeleton-docs/vox-output", // default value
                 0, // associated to 1st input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "xgraph_edit_distance",
                 dborl_parameter_type_info::FILEASSOC);


  // Pre-processing
  //: Should the model and query shape be normalized to standard size before computing distance?
  this->normalize_before_computing_distance_.set_values(this->param_list_, 
    "io", "normalize_before_computing_distance", 
    "[PREP] Should shapes be normalized before computing distance?", 
    false,
    false);
  

  //: Normalization value for shape size (square root of area) to scale the query and model shapes to
  this->norm_shape_size_.set_values(this->param_list_, 
    "io", "norm_shape_size", 
    "[PREP] Normalized shape size (square root of area) to scale the shapes to", 
    64.0f,
    64.0f);
  
  this->use_mirrored_shape_.set_values(this->param_list_, 
    "io", "use_mirrored_shape", 
    "[PREP] Try both query_shape and its mirrored shape and take the min?", 
    false,
    false);

  // Process parameters
  this->convert_sk2d_to_sksp_tol_.set_values(this->param_list_, 
    "io", "convert_sk2d_to_sksp_tol", 
    "[CONVERT] Tolerance for converting sk2d graph to sksp graph (pixels)", 
    0.5f,
    0.5f);

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

