//:
// \file
// \brief An example parameter set inheriting from dborl_algo_params
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 03/26/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_patch_match_params_h)
#define _dborl_patch_match_params_h

#include <dborl/algo/dborl_algo_params.h>
#include <dbskr/algo/io/dbskr_extract_patch_params.h>
#include <dbskr/algo/io/dbskr_detect_patch_params.h>
#include <dbskr/algo/io/dbskr_extract_shock_params.h>
#include <dbskr/algo/io/dbskr_match_shock_params.h>  // for dbskr_tree_edit_params2

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_patch_match_params : public dborl_algo_params
{
public:
  
  //: parameters from the example algo in ORL documentations for parameter handling
  dborl_parameter<vcl_string> db_index_prototype_;
  dborl_parameter<vcl_string> db_index_query_;

  dborl_parameter<bool> use_assoc_match_folder_;
  dborl_parameter<vcl_string> match_folder_assoc_;    // use_assoc_match_folder then use this folder otherwise create
  dborl_parameter<vcl_string> match_folder_to_create_;

  //: to find the saved patch storages of the prototypes
  dbskr_extract_patch_params2 patch_params_prototype_;
  dborl_parameter<vcl_string> patch_folder_assoc_prototype_;
  dborl_parameter<vcl_string> shock_folder_assoc_prototype_;  // required for quad algo

  //: to find saved patch storages of the queries
  dbskr_extract_patch_params2 patch_params_query_;
  dborl_parameter<vcl_string> patch_folder_assoc_query_;    
  dborl_parameter<vcl_string> shock_folder_assoc_query_;  // required for quad algo

  dborl_parameter<bool> save_detections_;       // save a file similar in format to the ground truth files and evaluation files
  dborl_parameter<bool> use_saved_detections_;       // use saved detections if available
  dborl_parameter<bool> compute_one_per_computer_;       // 

  dborl_parameter<bool> use_object_shock_query_;
  dborl_parameter<bool> use_object_shock_proto_;
  
  dbskr_tree_edit_params2 edit_params_;   // construct and later add with the defaults

  dbskr_detect_instance_params2 detection_params_;
  dbskr_evaluate_detection_params evaluate_params_;

  dborl_patch_match_params() : dborl_algo_params("dborl_patch_match") { 

    db_index_prototype_.set_values(param_list_, "io", "db_index_prototype", "prototype index file", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/99-db-flat-index-small.xml", 
      0, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC);

    db_index_query_.set_values(param_list_, "io", "db_index_query", "query index file", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/99-db-flat-index-small.xml", 
      1, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC);

    save_detections_.set_values(param_list_, "io", "save_detections", "save_detections?", false, false);
    use_saved_detections_.set_values(param_list_, "io", "use_saved_detections", "use_saved_detections?", false, false);
    compute_one_per_computer_.set_values(param_list_, "io", "compute_one_per_computer", "compute_one_per_computer?", false, false);

    use_assoc_match_folder_.set_values(param_list_, "io", "use_assoc_match_folder", "use_assoc_match_folder?", true, true);
    match_folder_assoc_.set_values(param_list_, "io", "match_folder_assoc", "match_folder_assoc", "",
      "/vision/projects/kimia/categorization/99-db/orl-exps/matches/prototypes-vs-q/", 
      1, dborl_parameter_system_info::NOT_DEFINED, "match_set", dborl_parameter_type_info::FILEASSOC);

    match_folder_to_create_.set_values(param_list_, 
      "io", 
      "match_folder_to_create", 
      "match_folder_to_create", 
      "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/matches/prototypes-vs-q/", 
      1, dborl_parameter_system_info::OUTPUT_FILE, "match_set",
      dborl_parameter_type_info::FILEASSOC);
    
    patch_folder_assoc_prototype_.set_values(param_list_, "io", "patch_folder_assoc_prototype", "patch_folder_assoc_prototype", "",
      "/vision/projects/kimia/categorization/99-db/orl-exps/patches/prototypes/", 
      1, dborl_parameter_system_info::NOT_DEFINED, "patch_set", dborl_parameter_type_info::FILEASSOC);

    patch_folder_assoc_query_.set_values(param_list_, "io", "patch_folder_assoc_query", "patch_folder_assoc_query", "",
      "/vision/projects/kimia/categorization/99-db/orl-exps/patches/queries/", 
      1, dborl_parameter_system_info::NOT_DEFINED, "patch_set", dborl_parameter_type_info::FILEASSOC);

    shock_folder_assoc_prototype_.set_values(param_list_, "io", "shock_folder_assoc_prototype", "shock_folder_assoc_prototype", "",
      "/vision/projects/kimia/categorization/99-db/orl-exps/shocks/prototypes/", 
      1, dborl_parameter_system_info::NOT_DEFINED, "shock_set", dborl_parameter_type_info::FILEASSOC);

    shock_folder_assoc_query_.set_values(param_list_, "io", "shock_folder_assoc_query", "shock_folder_assoc_query", "",
      "/vision/projects/kimia/categorization/99-db/orl-exps/shocks/queries/", 
      1, dborl_parameter_system_info::NOT_DEFINED, "shock_set", dborl_parameter_type_info::FILEASSOC);

    use_object_shock_query_.set_values(param_list_, "io", "use_object_shock_query", "use_object_shock_query?", false, false);
    use_object_shock_proto_.set_values(param_list_, "io", "use_object_shock_proto", "use_object_shock_proto?", false, false);
    
    patch_params_prototype_.algo_name_ = "proto_p_e";  // to construct output_file_postfix() as was constructed before
    add_params(patch_params_prototype_);

    patch_params_query_.algo_name_ = "query_p_e";  // to construct output_file_postfix() as was constructed before
    add_params(patch_params_query_);

    edit_params_.algo_name_ = "e_d";
    add_params(edit_params_);

    evaluate_params_.algo_name_ = "eval_det";
    add_params(evaluate_params_);

    detection_params_.algo_name_ = "det";
    add_params(detection_params_);

  }
};

#endif  //_dborl_patch_match_params_h
