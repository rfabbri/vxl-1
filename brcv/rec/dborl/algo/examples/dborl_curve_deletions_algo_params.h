//:
// \file
// \brief An example parameter set inheriting from dborl_algo_params
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_curve_deletions_algo_params_h)
#define _dborl_curve_deletions_algo_params_h

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_curve_deletions_algo_params : public dborl_algo_params
{
public:
  dborl_parameter<bool> delete_segments_;  
  dborl_parameter<bool> add_segments_;  // if both true deletes random segments and adds random line segments of total length perc_add_*total_length
  
  dborl_parameter<float> perc_del_;
  dborl_parameter<float> sigma_del_;
  dborl_parameter<float> divident_del_;

  dborl_parameter<float> perc_add_;
  dborl_parameter<float> sigma_add_;
  dborl_parameter<float> divident_add_;

  dborl_parameter<vcl_string> db_index_;
  dborl_parameter<vcl_string> cem_folder_to_create_;

  dborl_parameter<vcl_string> input_extension_;

  vcl_string output_file_postfix_;

  dborl_curve_deletions_algo_params(vcl_string algo_name) : dborl_algo_params(algo_name) { 
    delete_segments_.set_values(param_list_, "data", "delete_segments", "randomly delete segments?", true, true);
    add_segments_.set_values(param_list_, "data", "add_segments", "randomly add line segments?", true, true);
    
    perc_del_.set_values(param_list_, "data", "perc_del", "perc of total length to delete", 10.0f, 10.0f); 
    sigma_del_.set_values(param_list_, "data", "sigma_del", "sigma to find a random length for the deleted segment", 2, 2); 
    divident_del_.set_values(param_list_, "data", "divident_del", "divident to determine number of deleted segments", 8, 8); 

    perc_add_.set_values(param_list_, "data", "perc_add", "percentage of total length to add", 10.0f, 10.0f); 
    sigma_add_.set_values(param_list_, "data", "sigma_add", "sigma to find a random length for the added segment", 2, 2); 
    divident_add_.set_values(param_list_, "data", "divident_add", "divident to determine number of added segments", 8, 8); 

    db_index_.set_values(param_list_, "io", "db_index", "index file", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/99-db-flat-index-small.xml", 
      0, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC); 
    cem_folder_to_create_.set_values(param_list_,  "io",  "cem_folder_to_create",  "cem folder to create", "", 
      "/vision/projects/kimia/categorization/99-db/orl-exps/exp/out/cem_folder/", 
      0, dborl_parameter_system_info::OUTPUT_FILE, "boundary_set");

    input_extension_.set_values(param_list_, "data_io", "input_extension", "extension to concatanate input", ".con", ".con");
  
    update_output_file_postfix();
    
  }

  void update_output_file_postfix() {

    vcl_stringstream ss;
    ss << perc_del_();

    vcl_stringstream ss2;
    ss2 << sigma_del_();
    
    vcl_stringstream ss3;
    ss3 << divident_del_();    

    vcl_stringstream ss4;
    ss4 << perc_add_();
    
    vcl_stringstream ss5;
    ss5 << sigma_add_();
    
    vcl_stringstream ss6;
    ss6 << divident_add_();

    if (delete_segments_()) {
      if (add_segments_()) {
        output_file_postfix_ = "_del_" + ss.str() + "_" + ss2.str() + "_" + ss3.str() + "_add_" + ss4.str() + "_" + ss5.str() + "_" + ss6.str();
      } else {  // just delete
        output_file_postfix_ = "_del_" + ss.str() + "_" + ss2.str() + "_" + ss3.str();
      }
    } else {
      if (add_segments_()) { // just add
        output_file_postfix_ = "_add_" + ss4.str() + "_" + ss5.str() + "_" + ss6.str();
      }
    }

    vcl_cout << "output_file_postfix_: " << output_file_postfix_ << vcl_endl;
  }

};

#endif  //_dborl_curve_deletions_algo_params_h
