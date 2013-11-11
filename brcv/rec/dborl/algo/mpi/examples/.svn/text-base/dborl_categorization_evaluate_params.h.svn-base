//:
// \file
// \brief The parameter class for dborl_categorization_evaluate inherits from dborl_algo_params
//        handles all the io related to input.xml, status.xml, perf.xml, params.xml thanks to inheritance
//        all other parameters needed for dborl_categorization_evaluate is defined here and added to param_list_ of base class in the constructor
//        
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 01/30/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_categorization_evaluate_params_h)
#define _dborl_categorization_evaluate_params_h

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_categorization_evaluate_params : public dborl_algo_params
{
public:

  dborl_parameter<vcl_string> index_filename;
  dborl_parameter<vcl_string> output_directory;
  
  dborl_categorization_evaluate_params(vcl_string algo_name) : dborl_algo_params(algo_name) { 
    index_filename.set_values(param_list_, "io_data", "index_filename", "path of index file", "", "", -1, dborl_parameter_system_info::NOT_APPLICABLE, "flat_job", dborl_parameter_type_info::FILEASSOC); 
    output_directory.set_values(param_list_, "io_data", "output_directory", "output directory", "", "", -1, dborl_parameter_system_info::OUTPUT_FILE, "");
  }
};

#endif // _dborl_categorization_evaluate_params_h)
