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


#include "dborl_extract_shock.h"
#include "dborl_extract_shock_params.h"
#include <iostream>

int main(int argc, char *argv[]) {

  dborl_extract_shock_params_sptr params = new dborl_extract_shock_params("dborl_extract_shock");  // constructs with the default values;
  if (!params->parse_command_line_args(argc, argv))
    return 0;

  //: always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
    std::cout << "problems in writing params file to: " << params->print_params_file() << std::endl;

  if (params->exit_with_no_processing() || params->print_params_only())
    return 0;

  //: always call this method to actually parse the input parameter file whose name is extracted from the command line
  if (!params->parse_input_xml())
    return 0;

  dborl_extract_shock algo(params);
  if (!algo.parse_index(params->index_filename()))
    return 0;
 
  std::vector<int> things;
  if (!algo.initialize(things)) {
    std::cout << "Initialization failed\n";
    return -1;
  } else 
    std::cout << "initialized..";
  
  std::vector<char> results;
  for (unsigned i = 0; i < things.size(); i++) {
    char f;
    algo.process(things[i], f);
    results.push_back(f);
  }

  std::cout << " processed..";
  algo.finalize(results);
  std::cout << " finalized..\n";

  return 0;
}

