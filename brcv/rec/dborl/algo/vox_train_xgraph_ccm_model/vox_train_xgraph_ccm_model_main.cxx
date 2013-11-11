// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_ccmetry_and_ccm_models/vox_train_xgraph_ccm_model/vox_train_xgraph_ccm_model_main.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 9, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "vox_train_xgraph_ccm_model_params.h"
#include "vox_train_xgraph_ccm_model_params_sptr.h"
#include "vox_train_xgraph_ccm_model_embedded_files.h"

#include <vcl_cstdlib.h>

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <dborl/algo/dborl_search_index_file.h>
#include <dborl/dborl_index_query.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <dbul/dbul_random.h>
#include <vpl/vpl.h>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_train_xgraph_ccm_model_params_sptr params =
            new vox_train_xgraph_ccm_model_params("vox_train_xgraph_ccm_model");

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


    vcl_string output_dir = params->output_ccm_dir_();
    vcl_string pos_output_model_file = output_dir + "/object_ccm_pos_model.xml";
    vcl_string neg_output_model_file = output_dir + "/object_ccm_neg_model.xml";
    vcl_string overall_model_file = output_dir + "/object_ccm_overall_model.xml";

    vcl_cout << "Output pos ccm model file: " << pos_output_model_file << vcl_endl;
    vcl_cout << "Output neg ccm model file: " << neg_output_model_file << vcl_endl;

    if(!vul_file_exists(output_dir))
    {
        vul_file::make_directory(output_dir);
    }

    vox_train_xshock_ccm_model_process ccm_pro;
    set_process_parameters_of_bpro1(*params, ccm_pro, "ccm");
    ccm_pro.parameters()->set_value("-index_file", bpro1_filepath(params->index_filename_()));
    ccm_pro.parameters()->set_value("-pos_output_file", bpro1_filepath(pos_output_model_file));
    ccm_pro.parameters()->set_value("-neg_output_file", bpro1_filepath(neg_output_model_file));

    ccm_pro.clear_input();
    ccm_pro.clear_output();

    bool pro_status = ccm_pro.execute();

    if(!pro_status)
    {
        vcl_cerr << "Problem occurred while training ccm model!" << vcl_endl;
        return 1;
    }

    //THIS PART WILL BE CHANGED.
    vox_extract_embedded_ccm_files(params->matlab_file_extraction_path_().c_str());
    vcl_string matlab_command = "matlab -nodesktop -nosplash -r \"addpath " + params->matlab_file_extraction_path_();
    matlab_command += "; compute_ccm_stat_dist_v2('" + pos_output_model_file + "', '"+ neg_output_model_file +
            "', '"+ params->bfrags_to_ignore_() +"', '"+ overall_model_file + "'); exit;\"";
    vcl_cout << "MATLAB command: " << matlab_command << vcl_endl;
    vcl_system(matlab_command.c_str());
    //
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

