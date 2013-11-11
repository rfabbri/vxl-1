//:
// \file
// \brief Parameters for the shock match evaluator on VOX
//
// \author Anil Usumezbas
// \date 04/05/2009
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#ifndef dborl_create_shock_sim_params_h_
#define dborl_create_shock_sim_params_h_

#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

class dborl_create_shock_sim_params : public dborl_algo_params
{
public:
  vcl_string algo_abbreviation_;
  
  //io parameters  
  dborl_parameter<vcl_string> input_dir_;    // passes the folder of the input object
  dborl_parameter<vcl_string> input_name_;

  dborl_parameter<vcl_string> output_dir_;          // if written to this folder as opposed to object folder then the edge map gets associated to the input object
                                                    // if nothing is written here, nothing gets associated

  //parameter for the index file
  dborl_parameter<vcl_string> index_filename_;

  //: constructor
  dborl_create_shock_sim_params (vcl_string algo_name) : dborl_algo_params(algo_name) 
  { 
    algo_abbreviation_ = "create_shock_sim";

    input_dir_.set_values (param_list_, "io", "input_dir", "input object", "", 
      "/vision/projects/kimia/categorization/", //default directory
      0,   // for 1th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR);

    input_name_.set_values (param_list_, "io", "input_name", "Input object name", "dummy", "dummy", 
      0,   // for 1th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    output_dir_.set_values(param_list_,  "io",  "output_folder",  "output folder to store results", "create_shock_sim", 
      "create_shock_sim", 
      0,   // associate to 0th input object
      dborl_parameter_system_info::OUTPUT_FILE, "similarity_matrix", dborl_parameter_type_info::FILEASSOC);

    index_filename_.set_values(param_list_, "io_data", "index_filename", "path of the index file", "", "", 0, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC);

  }

};

#endif  //dborl_create_shock_sim_params_h_

