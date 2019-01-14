// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_associate_groundtruth_xgraphs/vox_associate_groundtruth_xgraphs_main.cxx

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

#include "vox_associate_groundtruth_xgraphs_params.h"
#include "vox_associate_groundtruth_xgraphs_params_sptr.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <buld/buld_dir_file.h>
#include <vpl/vpl.h>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_associate_groundtruth_xgraphs_params_sptr params =
            new vox_associate_groundtruth_xgraphs_params("vox_associate_groundtruth_xgraphs");

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

    std::string object_name = params->input_object_name_();
    std::string input_file_name = params->input_xgraph_list_file_();

    std::ifstream xlists(input_file_name.c_str());

    std::string line;

    if (xlists.is_open())
    {
        while (! xlists.eof() )
        {
            getline(xlists, line);
            std::string query_object_name = line.substr(0, object_name.length());
            if(object_name == query_object_name)
            {
                std::string xgraph_file = line.substr(object_name.length() + 1);
                std::cout << "File to be associated: " << xgraph_file << std::endl;

                if(!vul_file_exists(params->output_xgraph_dir_()))
                {
                    vul_file::make_directory(params->output_xgraph_dir_());
                }
                std::string dest_file_name = params->output_xgraph_dir_() + "/" + vul_file::strip_directory(xgraph_file);
                std::cout << "Assoc file:" << dest_file_name << std::endl;

                bool copy_status = buld_copy_file(xgraph_file, dest_file_name);
                if(!copy_status)
                {
                    std::cerr << "Association cannot be completed. Check paths:" << std::endl;
                    std::cerr << xgraph_file << std::endl;
                    std::cerr << dest_file_name << std::endl;
                    if(vul_file_exists(dest_file_name))
                    {
                        vpl_unlink(dest_file_name.c_str());
                        vpl_rmdir(params->output_xgraph_dir_().c_str());
                    }
                    return 1;
                }
                break;
            }
        }
    }
    else
    {
        std::cerr << "Error: cannot open file: " << input_file_name << std::endl;
        return 1;
    }

    xlists.close();


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

