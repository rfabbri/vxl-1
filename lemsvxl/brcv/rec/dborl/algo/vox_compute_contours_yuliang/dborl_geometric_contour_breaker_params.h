//:
// \file
// \brief parameter set inheriting from dborl_algo_params
//
// \author Caio SOUZA
// \date 03/13/17
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_geometric_contour_breaker_params_h)
#define _dborl_geometric_contour_breaker_params_h

#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

#include <dbdet/pro/dbdet_contour_breaker_geometric_process.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_geometric_contour_breaker_params : public dborl_algo_params
{
public:
  dborl_parameter<bool> save_fragments_;  
  dborl_parameter<bool> save_to_object_folder_;  // if both true deletes random segments and adds random line segments of total length perc_add_*total_length
  
  dborl_parameter<vcl_string> input_object_name_;
  dborl_parameter<vcl_string> input_object_dir_;    // passes the folder of the input object
  dborl_parameter<vcl_string> input_extension_;     // extension of the input image

  dborl_parameter<vcl_string> output_extension_;

  dborl_parameter<vcl_string> output_cem_folder_;  // if written to this folder as opposed to object folder then the edge map gets associated to the input object
                                                    // if nothing is written here, nothing gets associated

  vcl_string algo_abbreviation_;

  dborl_geometric_contour_breaker_params(vcl_string algo_name) : dborl_algo_params(algo_name) { 

    algo_abbreviation_ = "cbg";
  
    input_object_name_.set_values(param_list_, "io", "input_object_name", "input_object name", "@IMGBASENAME@", "@IMGBASENAME@", 
      0,   // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    input_object_dir_.set_values(param_list_, "io", "input_object_dir", "input object", "@INPUTDIR@", 
      "@INPUTDIR@", 
      0,   // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR); 

    input_extension_.set_values(param_list_, "io", "input_extension", "input image extension", "@IMGEXT@", "@IMGEXT@");
    output_extension_.set_values(param_list_, "io", "output_extension", "output extension", ".cem", ".cem");
    
    output_cem_folder_.set_values(param_list_,  "io",  "output_cem_folder",  "output folder to write .cem", "@OUTPUTDIR@", 
      "@OUTPUTDIR@", 
      0,   // associate to 0th input object
      dborl_parameter_system_info::OUTPUT_FILE, "sel", dborl_parameter_type_info::FILEASSOC);
    
    //: add the parameters of the dbdet_third_order_edge_detector_process
    dbdet_contour_breaker_geometric_process pro;
    vcl_vector<bpro1_param*> pars = pro.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++) {
        param_list_.push_back(convert_parameter_from_bpro1(algo_abbreviation_, 
                                                           "[" + algo_abbreviation_ + "]",
                                                           pars[i]));
    }
    
  }

};

#endif  //_dborl_geometric_contour_breaker_params_h
