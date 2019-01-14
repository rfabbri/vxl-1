//:
// \file
// \author Maruthi Narayanan ( mn@lems.brown.edu )
// \date Feb 26, 2010
// \brief An algorithm to construct graphs
//  
// \verbatim
// \endverbatim


#include "dborl_graph_categorization_params.h"
#include "dborl_graph_categorization_params_sptr.h"
#include <proximity_graph/dborl_graph_categorization.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <sstream>

int main(int argc, char *argv[]) 
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    // construct parameters with the default values;
    dborl_graph_categorization_params_sptr params = 
        new dborl_graph_categorization_params("dborl_graph_categorization");  
  
    // parse the command line arguments
    if (!params->parse_command_line_args(argc, argv))
        return 1;

    //: always print the params file if an executable to work with ORL web 
    // interface
    if (!params->print_params_xml(params->print_params_file()))
    {
        std::cerr << "problems in writing params file to: " 
                 << params->print_params_file() << std::endl;
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


    // Grab some generic parameters
    std::string graph_file = params->dataset_dir_()+"/"+
        params->graph_file_();
    std::string exemplar_dataset_file = params->dataset_dir_()+"/"+
        params->exemplar_sim_matrix_();
    std::string query_dataset_file = params->dataset_dir_()+"/"+
        params->query_sim_matrix_();
    std::string query_label_file = params->dataset_dir_()+"/"+
        params->query_label_file_();

    std::cout<<"Graph dataset  file: "<<graph_file<<std::endl;
    std::cout<<"Exemp dataset  file: "<<exemplar_dataset_file<<std::endl;
    std::cout<<"Query dataset file:  "<<query_dataset_file<<std::endl;
    std::cout<<"Query label    file: "<<query_label_file<<std::endl;
  
    // Create ouput file name
    std::stringstream graph_value;
    std::string output_string;
    graph_value<<params->beta_();
    output_string=graph_value.str();
        
    if ( output_string.length() > 1 )
    {
        if ( output_string.find(".") )
        {
            std::cout<<output_string<<std::endl;
            output_string.replace(output_string.find("."),1,"_");
            
        }
    }

    std::string output_file_name = params->output_dir_() + "/" +
        "beta_" + output_string + "_graph_embed_cat.txt";

    std::cout<<"Writing results to "<<  output_file_name << std::endl;
    std::cout<<std::endl;

    dborl_graph_categorization graph;
    graph.graph_categorize(graph_file,
                           exemplar_dataset_file,
                           query_dataset_file,
                           query_label_file,
                           output_file_name,
                           params->beta_() );

    double vox_time = t.real()/1000.0;
    t.mark();
    std::cout<<std::endl;
    std::cout<<"************ Time taken: "<<vox_time<<" sec"<<std::endl;


    return 0;
}

