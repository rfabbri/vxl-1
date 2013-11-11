//:
// \file
// \author Maruthi Narayanan ( mn@lems.brown.edu )
// \date June 17, 2009
// \brief An algorithm to match shocks
//        This can be either shock graphs from shock patches
//        or just normal shock graphs
//  
// \verbatim
// \endverbatim


#include "vox_match_shock_graphs_params.h"
#include "vox_match_shock_graphs_params_sptr.h"
#include "vox_match_shock_graphs_one_to_many.h"
#include "vox_match_shock_patches.h"
#include <vul/vul_file.h>
#include <vul/vul_timer.h>


int main(int argc, char *argv[]) 
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    // construct parameters with the default values;
    vox_match_shock_graphs_params_sptr params = 
        new vox_match_shock_graphs_params("vox_match_shock_graphs");  
  
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
 
    // Determine which type of matching we are doing 
    // one to many - index file versus data
    // dataset to dataset - shock patch approach
    // If the index filename is empty we are doing dataset to dataset
    // otherwise one to many

    //******************** Match Shock Graphs ****************************
    vcl_cout<<"************  Match Shock Graphs  *************"<<vcl_endl;

    bool proc_status(false);

    if ( vul_file::exists(params->index_filename_()))
    {
        vox_match_shock_graphs_one_to_many algo(params);
        proc_status = algo.process();
    }
    else
    {
        vox_match_shock_patches algo(params);
        proc_status = algo.process();
    }

    if ( !proc_status )
    {

        vcl_cerr<<"Shock Graph Matching Failed"<<vcl_endl;
        
        // Finish off by updating status results
        params->percent_completed = 0.0f;
        params->exit_code = 1;
        params->exit_message = "Shock Matching Failed !";
        params->print_status_xml();

        return 1;
    }
    else
    {
        // Finish off by updating status results
        params->percent_completed = 100.0f;
        params->exit_code = 0;
        params->exit_message = "Shock Matching Finished Succesfully";
        params->print_status_xml();

       

    }
  
    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;


    return 0;
}

