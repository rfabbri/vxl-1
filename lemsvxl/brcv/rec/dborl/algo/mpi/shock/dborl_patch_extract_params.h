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

#if !defined(_dborl_patch_extract_params_h)
#define _dborl_patch_extract_params_h

#include <dborl/algo/dborl_algo_params.h>
#include <dbskr/algo/io/dbskr_extract_patch_params.h>
#include <dbskr/algo/io/dbskr_extract_shock_params.h>
#include <dbskr/dbskr_sm_cor.h>  // for dbskr_tree_edit_params2

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_patch_extract_params : public dborl_algo_params
{
public:
  
  //: parameters from the example algo in ORL documentations for parameter handling
  dborl_parameter<vcl_string> db_index_;
  dborl_parameter<vcl_string> patch_folder_;

  dborl_parameter<bool> use_object_shock_;  // if false use assoc shocks
  dborl_parameter<vcl_string> assoc_shock_folder_;

  dbskr_extract_patch_params2 extract_patch_;
  dborl_parameter<vcl_string> image_extention_; // if images are used during patch extraction

  dborl_patch_extract_params() : dborl_algo_params("patch_ext") { 

    db_index_.set_values(param_list_, "io", "db_index", "index file", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/99-db-flat-index-small.xml", 
      0, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC); 

    patch_folder_.set_values(param_list_, "io", 
      "patch_folder", "patch folder", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/output_patches/", 
      0, dborl_parameter_system_info::OUTPUT_FILE, "patch_set", dborl_parameter_type_info::FILEASSOC);
    
    //: if true use the shocks in the object folders
    use_object_shock_.set_values(param_list_, "obj", "use_object_shock", "use shocks in obj folders?", true, true);

    assoc_shock_folder_.set_values(param_list_, "shock", "assoc_shock_folder", "shock folder assoc", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/shocks/folder/", 0, 
      dborl_parameter_system_info::NOT_DEFINED, "shock_set", dborl_parameter_type_info::FILEASSOC); 

    image_extention_.set_values(param_list_, "obj", "image_extention", "image extensions in obj folders?", ".pgm", ".pgm");
    
    extract_patch_.algo_name_ = "p_e";
    add_params(extract_patch_);
  }
};

#endif  //_dborl_patch_extract_params_h
