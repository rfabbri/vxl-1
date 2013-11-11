//:
// \file



#include "vox_match_composite_fragments_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dbskfg/pro/dbskfg_match_bag_of_fragments_process.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_match_composite_fragments_params::
vox_match_composite_fragments_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name),
    edit_params_(),
    tag_match_composite_fragments_("Match_Composite_Fragments")
{ 


  //: model image directory
  this->model_image_.set_values
      (this->param_list_, 
       "io", 
       "model_image", 
       "-io: input path to image of model  ", 
       "", "");

  //: query image directory
  this->query_image_.set_values
      (this->param_list_, 
       "io", 
       "query_image", 
       "-io: input path to image of query ", 
       "", "");

  // add the parameters of the extract shock patches process
  dbskfg_match_bag_of_fragments_process pro1;
  vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      this->param_list_.push_back(convert_parameter_from_bpro1
                                  (tag_match_composite_fragments_,
                                   "[" + tag_match_composite_fragments_ + "]",
                                   pars[i]));
  }

  // // // Which type of normalization to use?
  // // this->norm_reconstruct_.set_values
  // //     (this->param_list_, "edit_data", "edit_data-norm_reconst", 
  // //   "[edit-data] Normalize using reconst bnd length (else total splice cost)?", 
  // //     false, false);

  // // // Save result to the object folder?
  // // this->save_to_object_folder_.set_values(this->param_list_, "io", 
  // //   "save_to_object_folder", "-io: save result to object folder ?", 
  // //                                         false, false);

  // // //: Name of input model
  // // this->model_object_name_.
  // //     set_values(this->param_list_, 
  // //                "io", "model_object_name",
  // //                "model_object_name", "dummy", "dymmy",
  // //                0, // for 1th input object
  // //                dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  // // //: passes the folder of the input model
  // // this->model_object_dir_.
  // //     set_values(this->param_list_, 
  // //                "io", "model_object_dir",
  // //                "model object folder", "", 
  // //                "/vision/images/misc/object",
  // //                0, // for 1th input object
  // //                dborl_parameter_system_info::INPUT_OBJECT_DIR);

  // // //: Name of input query
  // // this->query_object_name_.
  // //     set_values(this->param_list_, 
  // //                "io", "query_object_name",
  // //                "query_object_name", "dummy", "dymmy",
  // //                1, // for 1th input object
  // //                dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  // // //: passes the folder of the input query
  // // this->query_object_dir_.
  // //     set_values(this->param_list_, 
  // //                "io", "query_object_dir",
  // //                "query object folder", "", 
  // //                "/vision/images/misc/object",
  // //                1, // for 1th input object
  // //                dborl_parameter_system_info::INPUT_OBJECT_DIR);

  // // //: passes the input association directory
  // // this->query_assoc_dir_.set_values(param_list_, 
  // //                                    "io", "query_assoc_dir", 
  // //                                    "path of the assoc filename", "", "", 
  // //                                    1, // for 1th input object
  // //                                    dborl_parameter_system_info::NOT_DEFINED, 
  // //                                    "cgraph_fragment_storage", 
  // //                                    dborl_parameter_type_info::FILEASSOC);

 
  // // //: extension of the input composite fragment graph_extension
  // // this->input_composite_fragments_extension_.set_values
  // //     (this->param_list_, 
  // //      "io", 
  // //      "input_composite_fragments_extension", 
  // //      "-io: input composite fragments extension ", 
  // //      "_cgraph_fragments", "_cgraph_fragments");

  // // // Should we write out normalized or unnormalized costs
  // // this->write_matching_norm_.set_values
  // //     (this->param_list_, "io", 
  // //      "write_matching_norm", 
  // //      "-io: write out normalized results (default is unormalized) ?", 
  // //      false, false);

  // // //: extension of the output cgraph matching results
  // // this->output_match_extension_.set_values
  // //     (this->param_list_, 
  // //      "io", 
  // //      "output_match_extension", 
  // //      "-io: output match extension ( .xml ) ", 
  // //      ".xml", ".xml");

  
  // // // Output folder for composite graph matches
  // // this->output_cgraph_match_folder_.
  // //     set_values(this->param_list_, "io", 
  // //                "output_cgraph_match_folder", 
  // //                "output folder to write matching of bag of fragments", "",
  // //                "/vision/projects/kimia/categorization/output",
  // //                1, // associated to 0th input object
  // //                dborl_parameter_system_info::OUTPUT_FILE,
  // //                "cgraph_fragment_match_file",
  // //                dborl_parameter_type_info::FILEASSOC);   

  // // Add the edit params in
  // add_params(edit_params_);

}

