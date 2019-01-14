//:
// \file
// \author Maruthi Narayanan ( mn@lems.brown.edu )
// \date Feb 27, 2010
// \brief An algorithm to search graphs
//  
// \verbatim
// \endverbatim


#include "dborl_find_nn_params.h"
#include "dborl_find_nn_params_sptr.h"
#include <proximity_graph/dborl_graph_wave_propagation.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>


int main(int argc, char *argv[]) 
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    // construct parameters with the default values;
    dborl_find_nn_params_sptr params = 
        new dborl_find_nn_params("dborl_find_nn");  
  
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

    // Grab search type
    dborl_graph_wave_propagation::Search_type search_type = 
        static_cast<dborl_graph_wave_propagation::Search_type>(
            params->search_type_());
        
    // Do batch processing

    // batch processing grab all graph files
    std::vector<std::string> batch_graph_files;

    // batch processing grab all matrix files
    std::vector<std::string> batch_mat_files;

    // batch processing grab all seeds files
    std::vector<std::string> batch_seed_files;

    // Open the file
    std::ifstream file_opener;
    file_opener.open((params->dataset_dir_()+"/"+
                      params->batch_files_graphs_list_()).c_str());

    // Grab graphs
         
    // Read each graph file
    std::string temp;
    while(file_opener)
    {
        getline(file_opener,temp);
        batch_graph_files.push_back(params->dataset_dir_()+"/"+temp);
    }

    //Delete last element
    batch_graph_files.pop_back();
       
    //Close file
    file_opener.close();

    // Grab matrices

    //Rebind stream
    file_opener.open((params->dataset_dir_()+"/"+
                      params->batch_files_matrices_list_()).c_str());
        
    // Read each matrix name
    while(file_opener)
    {
        getline(file_opener,temp);
        batch_mat_files.push_back(params->dataset_dir_()+"/"+temp);
    }

    // Delete last element
    batch_mat_files.pop_back();
       
    // Close file
    file_opener.close();
   
    // Grab seeds

    // Only open seeds if doing fnn search
    if ( params->action_type_() == 0 )
    {
    
    
        //Rebind stream
        file_opener.open((params->seeds_dir_()+"/"+
                          params->batch_seeds_file_()).c_str());

        // Read each matrix name
        while(file_opener)
        {
            getline(file_opener,temp);
            batch_seed_files.push_back(params->seeds_dir_()+"/"+temp);
        }


        // Delete last element
        batch_seed_files.pop_back();
       
        // Close file
        file_opener.close();
    }

    //Loop over all batch files
    for(unsigned int i(0); i < batch_graph_files.size(); ++i)
    {

        // Grab file name
        std::string input_graph = vul_file::strip_extension
            (vul_file::strip_directory(batch_graph_files[i]));

        
        std::string query_labels = params->dataset_dir_()+"/"+
            params->query_label_file_();

        std::cout<<"Processing file:" << batch_graph_files[i]<<std::endl;
        std::cout<<"Using matrix:" << batch_mat_files[i]<<std::endl;
        
        if ( params->action_type_() == 0 )
        {

            // Create output name
            std::string stats_file_name = params->output_dir_()+
                "/"+input_graph+"_fnn_stats.txt";        
            std::cout<<"Writing results to:"<< stats_file_name<<std::endl;

            std::cout<<"Using seeds file:"<< batch_seed_files[i]<<std::endl;
            std::cout<<"Using query file:"<< query_labels <<std::endl;
            std::cout<<"Find nearest neighbor"<<std::endl;
            std::cout<<std::endl;

            // Do query searching
            dborl_graph_wave_propagation graph_search;
            graph_search.find_nn(batch_graph_files[i],
                                 batch_mat_files[i],
                                 query_labels,
                                 batch_seed_files[i],
                                 stats_file_name,
                                 search_type,
                                 params->tau_(),
                                 params->verbose_());
  
     


        }
        else if( params->action_type_() == 1 )
        {
            std::cout<<"Testing node reachability on graph"<<std::endl;
            
            // Create output name
            std::string stats_file_name = params->output_dir_()+
                "/"+input_graph+"_tr_stats.txt"; 
            std::cout<<"Writing results to:"<< stats_file_name<<std::endl;

            // Create object to do graph searching
            dborl_graph_wave_propagation graph_search;
            graph_search.test_reachability(batch_graph_files[i],
                                           batch_mat_files[i],
                                           stats_file_name,
                                           search_type,
                                           params->tau_(),
                                           params->verbose_());
        }
        else
        {

            std::cout<<"Testing optmal seed selection"<<std::endl;


            // Create output name
            std::string stats_file_name = params->output_dir_()+
                "/"+input_graph+"_oss_stats.txt"; 
            std::cout<<"Writing results to:"<< stats_file_name<<std::endl;

            // Create object to do graph searching
            dborl_graph_wave_propagation graph_search;
            graph_search.optimal_seed_selection(batch_graph_files[i],
                                                batch_mat_files[i],
                                                stats_file_name,
                                                search_type,
                                                params->tau_(),
                                                params->verbose_());



        }
    }

    double vox_time = t.real()/1000.0;
    t.mark();
    std::cout<<std::endl;
    std::cout<<"************ Time taken: "<<vox_time<<" sec"<<std::endl;


    return 0;
}

