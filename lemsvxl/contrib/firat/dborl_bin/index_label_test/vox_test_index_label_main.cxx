// This is /lemsvxl/contrib/firat/dborl_bin/index_label_test/vox_test_index_label_main.cxx

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

#include "vox_test_index_label_params.h"
#include "vox_test_index_label_params_sptr.h"

#include <iostream>
#include <fstream>
#include <vul/vul_timer.h>
#include <string>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_test_index_label_params_sptr params =
            new vox_test_index_label_params("vox_test_index_label");

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


    std::cout << "My name is " << params->input_object_name_() << ". My assoc label is " << params->assoc_label_() << ". And this is my index file:" << std::endl;

    std::string index_file = params->index_filename_();
    std::ifstream indexs(index_file.c_str());

    std::string line;

    if (indexs.is_open())
    {
        while (! indexs.eof() )
        {
            getline (indexs,line);
            std::cout << line << std::endl;
        }
    }
    indexs.close();


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

