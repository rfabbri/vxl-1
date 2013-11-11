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

#if !defined(_dborl_shock_retrieval_params_h)
#define _dborl_shock_retrieval_params_h

#include <dborl/algo/dborl_algo_params.h>
#include <dbskr/algo/io/dbskr_extract_patch_params.h>
#include <dbskr/algo/io/dbskr_extract_shock_params.h>
#include <dbskr/dbskr_sm_cor.h>  // for dbskr_tree_edit_params2

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_shock_retrieval_params : public dborl_algo_params
{
public:
  
  //: parameters from the example algo in ORL documentations for parameter handling
  dborl_parameter<vcl_string> db_index_;
  dborl_parameter<vcl_string> output_folder_;
  //: will be concatanated at the end of the object names to find esf files e.g. 0.5-0.1.esf
  dborl_parameter<vcl_string> input_ext_;

  dborl_parameter<bool> normalize_;
  dborl_parameter<bool> norm_reconst_bound_;  // only one of these should be true
  dborl_parameter<bool> norm_tot_splice_cost_;
  dborl_parameter<bool> norm_con_arclength_;  // assumes con files are saved in object directories
  dborl_parameter<bool> save_matches_;  // assumes con files are saved in object directories
  dborl_parameter<bool> use_rho_;  // use similarity values to find nearest neighbors
  dborl_parameter<float> rho_;
  dborl_parameter<int> k_;   // if use_rho is false pick top k_ neighbors

  dborl_parameter<vcl_string> sim_matrix_folder_;
  dborl_parameter<bool> use_associated_sim_file_;
  dborl_parameter<vcl_string> sim_matrix_folder_to_create_;
  
  dbskr_tree_edit_params2 edit_params_;

  dborl_shock_retrieval_params() : dborl_algo_params("dborl_shock_retrieval") { 

    output_folder_.set_values(param_list_, "io", "output_folder", "output folder", "", "/vision/projects/kimia/categorization/99-db/orl-exps/shock_retrieval_output/", -1, dborl_parameter_system_info::OUTPUT_DIRECTORY);
    db_index_.set_values(param_list_, "io", "db_index", "path of index file", "", "/vision/projects/kimia/categorization/99-db/orl-exps/99-db-flat-index.xml", 0, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC); 
    
    sim_matrix_folder_.set_values(param_list_, "io", "sim_matrix_folder", 
      "associated similarity matrix folder", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/shock_retrieval_output/sim_matrix/", 
      0, 
      dborl_parameter_system_info::NOT_DEFINED, 
      "similarity_matrix", 
      dborl_parameter_type_info::FILEASSOC); 

    use_associated_sim_file_.set_values(param_list_, "io", "use_associated_sim_file", "if true use the associated sim file", false, false);
    sim_matrix_folder_to_create_.set_values(param_list_, 
      "io", 
      "sim_matrix_folder_to_create", 
      "the output similarity matrix folder if to be created", 
      "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/shock_retrieval_output/sim_matrix/", 
      0, dborl_parameter_system_info::OUTPUT_FILE, "similarity_matrix",
      dborl_parameter_type_info::FILEASSOC);

    input_ext_.set_values(param_list_, "data", "input_ext", "input extension, e.g. 0.5-0.1.esf", ".esf", ".esf");
    normalize_.set_values(param_list_, "data", "normalize", "normalize?", true, true);
    norm_reconst_bound_.set_values(param_list_, "data", "norm_reconst_bound", "use reconstructed boundary length for normalization?", true, true);
    norm_tot_splice_cost_.set_values(param_list_, "data", "norm_tot_splice_cost", "use total splice cost for normalization?", false, false);
    norm_con_arclength_.set_values(param_list_, "data", "norm_con_arclength", "use arclength of the con file for normalization?", false, false);
    save_matches_.set_values(param_list_, "data", "save_matches", "save shgm files?", true, true);
    use_rho_.set_values(param_list_, "data", "use_rho", "use rho? (otherwise k)", false, false);
    rho_.set_values(param_list_, "data", "rho", "rho: similarity threshold", 0.5f, 0.5f);
    k_.set_values(param_list_, "data", "k", "k: number of nearest neighbors", 5, 5);
    
    add_params(edit_params_);  // adds with the defaults

  }
};

#endif  //_dborl_shock_retrieval_params_h
