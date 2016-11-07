// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_assoc_geom_and_ccm_model/vox_assoc_geom_and_ccm_model_main.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Apr 5, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "vox_assoc_geom_and_ccm_model_params.h"
#include "vox_assoc_geom_and_ccm_model_params_sptr.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <dbul/dbul_dir_file.h>
#include <vpl/vpl.h>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_assoc_geom_and_ccm_model_params_sptr params =
            new vox_assoc_geom_and_ccm_model_params("vox_assoc_geom_and_ccm_model");

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

    if(!vul_file_exists(params->output_ccm_dir_()))
    {
        vul_file::make_directory(params->output_ccm_dir_());
    }
    vcl_string dest_file_name = params->output_ccm_dir_() + "/object_ccm_overall_model.xml";
    bool copy_status = dbul_copy_file(params->input_ccm_(), dest_file_name);
    if(!copy_status)
    {
        vcl_cerr << "Association cannot be completed. Check paths:" << vcl_endl;
        vcl_cerr << params->input_ccm_() << vcl_endl;
        vcl_cerr << dest_file_name << vcl_endl;
        if(vul_file_exists(params->output_ccm_dir_()))
        {
            vpl_unlink(dest_file_name.c_str());
            vpl_rmdir(params->output_ccm_dir_().c_str());
        }
        return 1;
    }

    if(!vul_file_exists(params->output_geom_dir_()))
    {
        vul_file::make_directory(params->output_geom_dir_());
    }
    vcl_string dest_file_name2 = params->output_geom_dir_() + "/object_geom_model.xml";
    copy_status = dbul_copy_file(params->input_geom_(), dest_file_name2);
    if(!copy_status)
    {
        vcl_cerr << "Association cannot be completed. Check paths:" << vcl_endl;
        vcl_cerr << params->input_geom_() << vcl_endl;
        vcl_cerr << dest_file_name2 << vcl_endl;
        if(vul_file_exists(params->output_geom_dir_()))
        {
            vpl_unlink(dest_file_name2.c_str());
            vpl_rmdir(params->output_geom_dir_().c_str());
        }
        return 1;
    }

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

