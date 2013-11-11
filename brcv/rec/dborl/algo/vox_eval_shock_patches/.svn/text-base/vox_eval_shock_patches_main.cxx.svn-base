//:
// \file
// \author Maruthi Narayanan ( mn@lems.brown.edu )
// \date July 15, 2009
// \brief An algorithm to evaluate shock patch detection results
//        returns TP,FP,FN,TN at a certain threshold
//  
// \verbatim
// \endverbatim


#include "vox_eval_shock_patches_params.h"
#include "vox_eval_shock_patches_params_sptr.h"
#include "vox_eval_shock_patches.h"
#include <vul/vul_file.h>
#include <vul/vul_timer.h>


int main(int argc, char *argv[]) 
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    // construct parameters with the default values;
    vox_eval_shock_patches_params_sptr params = 
        new vox_eval_shock_patches_params("vox_eval_shock_patches");  
  
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
 
    // Kick off algorithm
    vox_eval_shock_patches algo(params);
    bool status = algo.process();
  
    if (!status)
    {

        vcl_cerr<<"Evaluation Failed!!"<<vcl_endl;
        return 1;
    }

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;


    return 0;
}

