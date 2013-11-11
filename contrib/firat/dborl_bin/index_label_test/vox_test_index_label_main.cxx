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

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vul/vul_timer.h>
#include <vcl_string.h>

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
        vcl_cerr << "problems in writing params file to: " <<
        params->print_params_file() << vcl_endl;

    if (params->exit_with_no_processing() || params->print_params_only())
        return 0;

    //: always call this method to actually parse the input parameter file
    // whose name is extracted from the command line
    if (!params->parse_input_xml())
        return 1;


    vcl_cout << "My name is " << params->input_object_name_() << ". My assoc label is " << params->assoc_label_() << ". And this is my index file:" << vcl_endl;

    vcl_string index_file = params->index_filename_();
    vcl_ifstream indexs(index_file.c_str());

    vcl_string line;

    if (indexs.is_open())
    {
        while (! indexs.eof() )
        {
            getline (indexs,line);
            vcl_cout << line << vcl_endl;
        }
    }
    indexs.close();


    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;

    // Just to be safe lets flush everything
    vcl_cerr.flush();
    vcl_cout.flush();

    //Success we made it this far
    return 0;
}

