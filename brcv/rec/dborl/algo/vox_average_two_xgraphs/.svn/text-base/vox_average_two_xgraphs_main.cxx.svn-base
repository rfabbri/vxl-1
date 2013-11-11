// This is rec/dborl/algo/vox_average_two_xgraphs/vox_average_two_xgraphs_main.cxx

//:
// \file
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Mar 2, 2010
// \brief An algorithm to compute average of two shock graphs
//  
// \verbatim
// \endverbatim


#include "vox_average_two_xgraphs_params.h"
#include "vox_average_two_xgraphs_params_sptr.h"
#include "vox_average_two_xgraphs_one_to_one.h"
#include <vul/vul_file.h>
#include <vul/vul_timer.h>


int main(int argc, char *argv[]) 
{
  // Let time how long this takes
  // Start timer
  vul_timer t;

  // construct parameters with the default values;
  vox_average_two_xgraphs_params_sptr params = 
    new vox_average_two_xgraphs_params("vox_average_two_xgraphs");  

  // parse the command line arguments
  if (!params->parse_command_line_args(argc, argv))
    return 1;

  //: always print the params file if an executable to work with ORL web 
  // interface
  if (!params->print_params_xml(params->print_params_file()))
  {
    vcl_cerr << "problems in writing params file to: " 
      << params->print_params_file() << vcl_endl;
  }

  // exit if there is nothing else to do
  if (params->exit_with_no_processing() || params->print_params_only())
  {
    return 0;
  }

  //: always call this method to actually parse the input parameter file 
  // whose name is extracted from the command line
  if (!params->parse_input_xml())
  {
    return 1;
  }

  // Averaging two xgraphs
  vcl_cout<<"************  Average two xgraphs  *************\n";

  bool proc_status(false);

  vox_average_two_xgraphs_one_to_one algo(params);
  proc_status = algo.process();

  if ( !proc_status )
  {
    vcl_cerr<<"XGraph averaging failed ! failed.\n";

    // Finish off by updating status results
    params->percent_completed = 0.0f;
    params->exit_code = 1;
    params->exit_message = "XGraph averaging failed !";
    params->print_status_xml();

    return 1;
  }
  else
  {
    // Finish off by updating status results
    params->percent_completed = 100.0f;
    params->exit_code = 0;
    params->exit_message = "XGraph averaging finished succesfully";
    params->print_status_xml();
  }

  double vox_time = t.real()/1000.0;
  t.mark();
  vcl_cout<<vcl_endl;
  vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;

  return 0;
}

