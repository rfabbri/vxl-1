//:
// \file



#include "vox_match_shock_graphs_params.h"
#include <dborl/algo/dborl_algo_params.h>

//: Constructor
vox_match_shock_graphs_params::
vox_match_shock_graphs_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name),
    edit_params_()
{ 

  // Should pruning be performed on trees? 
  this->prune_tree_.set_values(this->param_list_, "edit_data", 
                                "edit_data-prune_tree", 
                                "[edit-data] should shock trees be pruned ?", 
                                false, false);

  // Shock graph pruning threshold
  this->prune_thres_.set_values(this->param_list_, "edit_data", 
                                "edit_data-prune_thres", 
                                "[edit-data] shock graph pruning threshold", 
                                0.8f, 0.8f);


  // Which type of normalization to use?
  this->norm_reconstruct_.set_values
      (this->param_list_, "edit_data", "edit_data-norm_reconst", 
    "[edit-data] Normalize using reconst bnd length (else total splice cost)?", 
      false, false);

  // Turn on/off mirror matching
  this->perform_mirror_matching_.set_values
      (this->param_list_, "edit_data", "edit_data-mirror_match",
       "[edit-data] perform mirror matching", false, false);

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
                                     "extrinsinc_shock_graph", 
                                     dborl_parameter_type_info::FILEASSOC);

 
  //: extension of the input shock_patch extension
  this->input_shock_patch_extension_.set_values
      (this->param_list_, 
       "io", 
       "input_shock_patch_extension", 
       "-io: input shock patch extension ", 
       "_shock_patches", "_shock_patches");

  // Should we write out normalized or unnormalized costs
  this->write_matching_norm_.set_values
      (this->param_list_, "io", 
       "write_matching_norm", 
       "-io: write out normalized results (default is unormalized) ?", 
       false, false);

  //: extension of the output shock matching results
  this->output_match_extension_.set_values
      (this->param_list_, 
       "io", 
       "output_match_extension", 
       "-io: output match extension ( .msghm, .bin, _norm.msghm ) ", 
       ".bin", ".bin");

  // Output shock folder for matches
  this->output_shock_match_folder_.
      set_values(this->param_list_, "io", 
                 "output_shock_match_folder", 
                 "output folder to write shock matching results", "",
                 "/vision/projects/kimia/categorization/output",
                 1, // associated to 0th input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "shock_match_file",
                 dborl_parameter_type_info::FILEASSOC);

  // Output shock folder for shock patch matches
  this->output_shock_patch_match_folder_.
      set_values(this->param_list_, "io", 
                 "output_shock_patch_match_folder", 
                 "output folder to write shock patch matching results", "",
                 "/vision/projects/kimia/categorization/output",
                 1, // associated to 0th input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "shock_patch_match_file",
                 dborl_parameter_type_info::FILEASSOC);

  // Index file for shock match computation
  this->index_filename_.set_values(param_list_, 
                                   "io", "index_filename", 
                                   "path of the index file", "", "", 
                                   0, 
                                   dborl_parameter_system_info::NOT_DEFINED, 
                                   "flat_image", 
                                   dborl_parameter_type_info::FILEASSOC);
   

  // Add the edit params in
  add_params(edit_params_);

}

