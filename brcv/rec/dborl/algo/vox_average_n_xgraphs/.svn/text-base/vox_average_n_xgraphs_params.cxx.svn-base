// This is rec/dborl/algo/vox_average_two_xgraphs/vox_average_n_xgraphs_params.cxx

//:
// /file



#include "vox_average_n_xgraphs_params.h"
#include <dborl/algo/dborl_algo_params.h>

//: Constructor
vox_average_n_xgraphs_params::
vox_average_n_xgraphs_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 
  //: Name of input model
  this->dataset_object_name_.set_values(this->param_list_, 
                 "io", 
                 "dataset_object_name",
                 "dataset_object_name", 
                 "dummy", 
                 "kimia_99_dog",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input model
  this->dataset_object_dir_.set_values(this->param_list_, 
                 "io", 
                 "dataset_object_dir",
                 "dataset_object_dir", 
                 "dummy", 
                 "V:/projects/kimia/shockshape/learnskeleton/data/vox-kimia-99-dataset",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);
 
  // Output shock folder for matches
  this->output_average_xgraph_folder_.set_values(this->param_list_, 
                 "io", 
                 "output_average_xgraph_folder", 
                 "output folder to write average xgraph results", 
                 "average_n_xgraph", // value
                 "D:/vision/docs/learnskeleton-docs/vox-output", // default value
                 0, // associated to 1st input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "average_n_xgraph",
                 dborl_parameter_type_info::FILEASSOC);

  // Index file for list of objects in dataset
  this->index_file_.set_values(param_list_, 
                                "io", 
                                "index_filename", 
                                "path of the index file", 
                                "dummy", 
                                "V:/projects/kimia/shockshape/learnskeleton/data/index_kimia_99_3_dogs.xml", 
                                0, 
                                dborl_parameter_system_info::NOT_DEFINED,
                                "flat_image", 
                                dborl_parameter_type_info::FILEASSOC);

  ////: Number of xgraphs to compute average from
  //this->num_xgraphs_to_average_.set_values(this->param_list_, 
  //  "io", "num_xgraphs_to_average", 
  //  "[DATA] Number of xgraphs to compute average", 
  //  5,
  //  5);



  //: Number of xgraphs to compute average from
  this->dataset_start_index_.set_values(this->param_list_, 
    "io", "dataset_start_index", 
    "[DATA] Start-index (in the dataset) to compute average [min=0]", 
    0,
    0);


  //: Number of xgraphs to compute average from
  this->dataset_end_index_.set_values(this->param_list_, 
    "io", "dataset_end_index", 
    "[DATA] End-index (in the dataset) to compute average [max= #shapes-1]", 
    4,
    4);  
  
  //: Averaging method
  // 0: curve shortening
  // 1: iterative merging
  // 2: best-exemplar
  // 3: use existing xgraph in the object
  // 4: choose a specific exemplar
  this->use_curve_shortening_.set_values(this->param_list_, 
    "io", "use_curve_shortening", 
    "[METHOD] 1. Use curve shortening", 
    true,
    false);

  this->use_iterative_merging_.set_values(this->param_list_, 
    "io", "use_iterative_merging", 
    "[METHOD] 2. Use iterative merging", 
    false,
    false);

  this->use_best_exemplar_.set_values(this->param_list_, 
    "io", "use_best_exemplar", 
    "[METHOD] 3. Use best exemplar", 
    false,
    false);

  this->use_existing_xgraph_in_object_folder_.set_values(this->param_list_, 
    "io", "use_existing_xgraph_in_object_folder", 
    "[METHOD] 4. Use existing xgraph in object folder", 
    false,
    false);

  this->use_specific_exemplar_.set_values(this->param_list_, 
    "io", "use_specific_exemplar", 
    "[METHOD] 5. Use specific exemplar in dataset", 
    false,
    true);

  this->extension_of_existing_xgraph_.set_values(this->param_list_, 
    "io", "extension_of_existing_xgraph", 
    "[METHOD] Extension of existing xgraph in object folder", 
    "-average.xml",
    "-average.xml");

  this->index_of_specific_exemplar_.set_values(this->param_list_, 
    "io", "index_of_specific_exemplar", 
    "[METHOD] Index of specific exemplar in dataset", 
    0,
    0);

  this->run_gradient_descent_at_the_end_.set_values(this->param_list_, 
    "io", "run_gradient_descent_at_the_end", 
    "[METHOD] Run gradient descent at the end", 
    true,
    true);

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

