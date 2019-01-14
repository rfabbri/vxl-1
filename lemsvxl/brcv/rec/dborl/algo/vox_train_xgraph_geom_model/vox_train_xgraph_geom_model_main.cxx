//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 5, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "vox_train_xgraph_geom_model_params.h"
#include "vox_train_xgraph_geom_model_params_sptr.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <dborl/algo/dborl_search_index_file.h>
#include <dborl/dborl_index_query.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <buld/buld_random.h>
#include <vpl/vpl.h>

bool create_temporary_gt_list(std::vector<std::string> assoc_files, const std::string& temp_gt_file)
{
    std::ofstream outs(temp_gt_file.c_str());
    if(!outs.is_open())
    {
        return false;
    }
    for(int i = 0; i < assoc_files.size(); i++)
    {
        vul_file_iterator fit(assoc_files[i] + "/*.xml");
        if(!fit)
        {
            std::cerr << "ERROR: xgraph xml file cannot be found under " << assoc_files[i] << std::endl;
            outs.close();
            vpl_unlink(temp_gt_file.c_str());
            return false;
        }
        outs << assoc_files[i] + "/" + fit.filename();
        if(i != assoc_files.size()-1)
        {
            outs << std::endl;
        }
    }
    outs.close();
    return true;
}

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_train_xgraph_geom_model_params_sptr params =
            new vox_train_xgraph_geom_model_params("vox_train_xgraph_geom_model");

    if (!params->parse_command_line_args(argc, argv))
        return 1;

    //: always print the params file if an executable to work with ORL web
    // interface
    if (!params->print_params_xml(params->print_params_file()))
        std::cerr << "problems in writing params file to: " <<
        params->print_params_file() << std::endl;

    if (params->exit_with_no_processing() || params->print_params_only())
        return 0;

    //: always call this method to actually parse the input parameter file
    // whose name is extracted from the command line
    if (!params->parse_input_xml())
        return 1;

    dborl_index_query index_query;
    index_query.add_assoc_file_label(params->xgraph_assoc_label_());
    index_query.add_assoc_file_type("groundtruth_xgraph_directory");

    std::string gt_object_list_file = params->input_gt_objects_file_();

    std::cout << "Groundtruth object list file: " << gt_object_list_file << std::endl;

    std::ifstream xlists(gt_object_list_file.c_str());
    std::string line;

    if(xlists.is_open())
    {
        while(!xlists.eof())
        {
            getline(xlists, line);
            index_query.add_object_name(line);
        }
    }
    else
    {
        std::cerr << "Error: cannot open file: " << gt_object_list_file << std::endl;
        return 1;
    }

    std::vector<std::string> assoc_files = dborl_get_assoc_file_paths(params->index_filename_(), index_query);

    int number_of_gt = assoc_files.size();

    std::cout << "Number of groundtruth objects is " << number_of_gt << std::endl;

    std::string temp_gt_file = params->temp_dir_() + "/temp_" + buld_get_random_alphanumeric_string(10) + ".txt";
    bool create_status = create_temporary_gt_list(assoc_files, temp_gt_file);

    if(!create_status)
    {
        std::cerr << "ERROR: Temporary file " << temp_gt_file << " cannot be created!" << std::endl;
        return 1;
    }

    std::cout << "Temporary file " << temp_gt_file << " has been created!" << std::endl;

    std::string output_dir = params->output_geom_dir_();
    std::string output_model_file = output_dir + "/object_geom_model.xml";

    std::cout << "Output geometry model file: " << output_model_file << std::endl;

    if(!vul_file_exists(output_dir))
    {
        vul_file::make_directory(output_dir);
    }

    dbsks_train_xshock_geom_model_process geom_pro;
    set_process_parameters_of_bpro1(*params, geom_pro, "geom");
    geom_pro.parameters()->set_value("-xgraph-list-file", bpro1_filepath(temp_gt_file));
    geom_pro.parameters()->set_value("-xgraph-folder", bpro1_filepath(""));
    geom_pro.parameters()->set_value("-output-file", bpro1_filepath(output_model_file));

    geom_pro.clear_input();
    geom_pro.clear_output();

    bool pro_status = geom_pro.execute();

    if(!pro_status)
    {
        std::cerr << "Problem occurred while training geometry model!" << std::endl;
        return 1;
    }

    vpl_unlink(temp_gt_file.c_str());
    std::cout << "Temporary file " << temp_gt_file << " has been deleted!" << std::endl;

    double vox_time = t.real()/1000.0;
    t.mark();
    std::cout<<std::endl;
    std::cout<<"************ Time taken: "<<vox_time<<" sec"<<std::endl;

    // Just to be safe lets flush everything
    std::cerr.flush();
    std::cout.flush();

    //Success we made it this far
    return 0;
}

