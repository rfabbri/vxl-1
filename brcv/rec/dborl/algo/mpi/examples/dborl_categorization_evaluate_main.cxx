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


#include "dborl_categorization_evaluate.h"
#include "dborl_categorization_evaluate_params.h"
#include <vcl_iostream.h>


int main(int argc, char *argv[]) {

  // Initialize defaults
  int return_status = 0;
  dborl_categorization_evaluate_params_sptr params = new dborl_categorization_evaluate_params("dborl_categorization_evaluate");
  dborl_categorization_evaluate e(params);  // just pass with the defaults to the constructor
  
  if (!e.parse_command_line(argc, argv))
    return 0;

  if (!e.parse(e.param_file_.c_str()))  // parses the index file as well
    return 0;

  e.process();
    
  return 0;

}

