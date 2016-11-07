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

#if !defined(_dborl_example_algo_params_h)
#define _dborl_example_algo_params_h

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_example_algo_params : public dborl_algo_params
{
public:
  dborl_parameter<float> a_float_;
  dborl_parameter<int> an_int_;
  dborl_parameter<vcl_string> filename_string_; 
  dborl_parameter<vcl_string> a_string_;
  dborl_parameter<float> another_float_;
  dborl_parameter<double> a_double_;
  dborl_parameter<bool> a_flag_;
  dborl_parameter<bool> another_flag_;    
  dborl_parameter<int> status_param_;
  dborl_parameter<short> a_short_;
  dborl_parameter<char> a_char_;
  
  //: parameters from the example algo in ORL documentations for parameter handling
  dborl_parameter<vcl_string> path_;
  dborl_parameter<vcl_string> object_name_;
  dborl_parameter<vcl_string> pixel_type_;
  dborl_parameter<vcl_string> filename_;
  dborl_parameter<vcl_string> iso_filename_;
  dborl_parameter<int> iters_;

  dborl_example_algo_params(vcl_string algo_name) : dborl_algo_params(algo_name) { 
    a_float_.set_values(param_list_, "data", "a_float", "set to a number between 0 and 10, tpr is this number dividied by 10", 5.0f, 5.0f); 
    an_int_.set_values(param_list_, "data", "an_int", "some int", 10, 0); 
    filename_string_.set_values(param_list_, "data", "filename_string", "some file", "datafile", "datafile"); 
    a_string_.set_values(param_list_, "data", "a_string", "some string", "somename", ""); 
    another_float_.set_values(param_list_, "other_data", "another_float", "set to a number between 0 and 10, fpr is this number divided by 10", 3.0f, 3.0f); 
    a_double_.set_values(param_list_, "other_data", "a_double", "a double", 50.0, 50.0); 
    a_flag_.set_values(param_list_, "other_data", "a_flag", "a flag", false, false); 
    another_flag_.set_values(param_list_, "data", "another_flag", "another flag", true, true); 
    status_param_.set_values(param_list_, "status", "status_param", "some counter to report status", 0, 0);
    a_short_.set_values(param_list_, "other_data", "a_short", "a short", 2, 2);
    a_char_.set_values(param_list_, "other_data", "a_char", "a char", 'a', 'a');

    path_.set_values(param_list_, "data", "path", "first input object's directory", "", "", 0, dborl_parameter_system_info::INPUT_OBJECT_DIR);
    object_name_.set_values(param_list_, "data", "object_name", "second input object's name", "", "", 1, dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    pixel_type_.set_values(param_list_, "data", "pixel_type", "Type of pixel", "uint16", "uint16");
    filename_.set_values(param_list_, "levelset_result", "filename", "performance output", "output.float", "output.float", -1, dborl_parameter_system_info::OUTPUT_FILE, "float");
    iso_filename_.set_values(param_list_, "iso_surface_object", "filename", "performance output", "output.obj", "output.obj", -1, dborl_parameter_system_info::OUTPUT_FILE, "obj");
    iters_.set_values(param_list_, "levelset_evolution_params", "iters", "Iterations", 0, 0, -1, dborl_parameter_system_info::NOT_DEFINED, "float");
  }
};

#endif  //_dborl_example_algo_params_h
