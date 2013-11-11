// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_train_xgraph_ccm_model/vox_train_xgraph_ccm_model_params.cxx

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
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_train_xgraph_ccm_model_params::vox_train_xgraph_ccm_model_params(vcl_string algo_name) : dborl_algo_params(algo_name)
{

    this->index_filename_.set_values(param_list_,
            "io", "index_filename",
            "path of the index file", "", "/vision/scratch/firat/vox_pyramids/index.xml",
            0,
            dborl_parameter_system_info::NOT_DEFINED,
            "flat_image",
            dborl_parameter_type_info::FILEASSOC);


    vox_train_xshock_ccm_model_process pro1;
    vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++)
    {
        bpro1_param* par = pars[i];
        vcl_string par_name = par->name();
        if(par_name != "-index_file" && par_name != "-pos_output_file" && par_name != "-neg_output_file")
        {
            param_list_.push_back(
                    convert_parameter_from_bpro1("ccm",
                            "[ccm] ",
                            par));
        }
    }

    // Output directory (if not object folder)
    this->output_ccm_dir_.set_values(this->param_list_, "io",
            "output_ccm_dir",
            "output xgraph ccm model directory", "",
            "/vision/scratch/firat/vox_pyramids/horses_ccm_model",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "xgraph_ccm_model_directory",
            dborl_parameter_type_info::FILEASSOC);

    this->matlab_file_extraction_path_.set_values(this->param_list_, "io",
            "matlab_file_extraction_path",
            "Path to extract required matlab files",
            "/vision/scratch/octave_tmp/ccm", "/vision/scratch/octave_tmp/ccm");

    this->bfrags_to_ignore_.set_values(this->param_list_, "data",
            "bfrags_to_ignore",
            "Input list of bnd contour fragments to ignore cost (separated by space commas)",
            "", "");
}
