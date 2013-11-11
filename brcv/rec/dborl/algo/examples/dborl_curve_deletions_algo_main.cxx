//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//


#include "dborl_curve_deletions_algo.h"
#include "dborl_curve_deletions_algo_params.h"
#include <vcl_iostream.h>

int main(int argc, char *argv[]) {

  dborl_curve_deletions_algo_params_sptr params = new dborl_curve_deletions_algo_params("dborl_curve_deletions_algo");  // constructs with the default values;
  if (!params->parse_command_line_args(argc, argv))
    return 0;

  //: always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
    vcl_cout << "problems in writing params file to: " << params->print_params_file() << vcl_endl;

  if (params->exit_with_no_processing() || params->print_params_only())
    return 0;

  //: always call this method to actually parse the input parameter file whose name is extracted from the command line
  if (!params->parse_input_xml())
    return 0;

  params->update_output_file_postfix();

  dborl_curve_deletions_algo algo(params);
 
  if (!algo.initialize()) {
    vcl_cout << "Initialization failed\n";
    return -1;
  } else 
    vcl_cout << "initialized..";
  
  algo.process();
  vcl_cout << " processed..";
  algo.finalize();
  vcl_cout << " finalized..\n";

  return 0;
}

