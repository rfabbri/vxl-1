//:
// \file
// \brief parameter set inheriting from dborl_algo_params
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 05/08/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_third_order_edge_det_params_h)
#define _dborl_third_order_edge_det_params_h

#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_third_order_edge_det_params : public dborl_algo_params
{
public:
  dborl_parameter<bool> save_edges_;  
  dborl_parameter<bool> save_to_object_folder_;  // if both true deletes random segments and adds random line segments of total length perc_add_*total_length
  
  dborl_parameter<vcl_string> input_object_name_;
  dborl_parameter<vcl_string> input_object_dir_;    // passes the folder of the input object
  dborl_parameter<vcl_string> input_extension_;     // extension of the input image

  dborl_parameter<vcl_string> output_extension_;

  dborl_parameter<vcl_string> output_edge_folder_;  // if written to this folder as opposed to object folder then the edge map gets associated to the input object
                                                    // if nothing is written here, nothing gets associated

  vcl_string algo_abbreviation_;

  dborl_third_order_edge_det_params(vcl_string algo_name) : dborl_algo_params(algo_name) { 

    algo_abbreviation_ = "toe";

    save_edges_.set_values(param_list_, "io", "save_edges", "save edges?", true, true);
    save_to_object_folder_.set_values(param_list_, "io", "save_to_object_folder", "save to object folder?", false, false);
  
    input_object_name_.set_values(param_list_, "io", "input_object_name", "input_object name", "dummy", "dummy", 
      0,   // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    input_object_dir_.set_values(param_list_, "io", "input_object_dir", "input object", "", 
      "/vision/images/misc/object/", 
      0,   // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR); 

    input_extension_.set_values(param_list_, "io", "input_extension", "input image extension", ".jpg", ".jpg");
    output_extension_.set_values(param_list_, "io", "output_extension", "output extension", ".edg", ".edg");
    
    output_edge_folder_.set_values(param_list_,  "io",  "output_edge_folder",  "output folder to write edge map", "", 
      "/vision/projects/kimia/categorization/output/", 
      0,   // associate to 0th input object
      dborl_parameter_system_info::OUTPUT_FILE, "edge_map", dborl_parameter_type_info::FILEASSOC);
    
    //: add the parameters of the dbdet_third_order_edge_detector_process
    dbdet_third_order_edge_detector_process pro;
    vcl_vector<bpro1_param*> pars = pro.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++) {
        param_list_.push_back(convert_parameter_from_bpro1(algo_abbreviation_, 
                                                           "[" + algo_abbreviation_ + "]",
                                                           pars[i]));
    }
    
  }
/*
  void set_process_parameters(dbdet_third_order_edge_detector_process& pro) {

    for (unsigned i = 0; i < param_list_.size(); i++) {
      if (param_list_[i]->param_group().compare(algo_abbreviation_) != 0)
        continue;
      vcl_string name = param_list_[i]->param_name();
      name = name.substr(algo_abbreviation_.size(), name.length());
      if (param_list_[i]->type_info() == dborl_parameter_type_info::PATH) {
        bpro1_filepath path(param_list_[i]->value_str());
        pro.parameters()->set_value(name, path);
      } else if (param_list_[i]->type_info() == dborl_parameter_type_info::FLAG) {
        vcl_string val_str = param_list_[i]->value_str();
        val_str.compare("off") == 0 ? pro.parameters()->set_value(name, false) : pro.parameters()->set_value(name, true);
      } else 
        bpro1_parameters_set_value_from_str((*pro.parameters()), name, param_list_[i]->value_str());
    }
  }
*/
};

#endif  //_dborl_third_order_edge_det_params_h
