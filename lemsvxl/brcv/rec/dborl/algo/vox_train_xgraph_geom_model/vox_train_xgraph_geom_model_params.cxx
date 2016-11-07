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
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_train_xgraph_geom_model_params::vox_train_xgraph_geom_model_params(vcl_string algo_name) : dborl_algo_params(algo_name)
{

    this->index_filename_.set_values(param_list_,
            "io", "index_filename",
            "path of the index file", "", "/vision/scratch/firat/vox_pyramids/index.xml",
            0,
            dborl_parameter_system_info::NOT_DEFINED,
            "flat_image",
            dborl_parameter_type_info::FILEASSOC);

    this->input_gt_objects_file_.set_values(this->param_list_, "io",
            "input_gt_objects_file",
            "Path to the input file containing the list of groundtruth objects",
            "/vision/scratch/firat/weizmann_xshock/gt-objects.txt", "/vision/scratch/firat/weizmann_xshock/gt-objects.txt");

    this->xgraph_assoc_label_.set_values(this->param_list_, "io",
            "xgraph_assoc_label",
            "Ground truth xgraph directory name",
            "weizmann_gt_xgraph", "weizmann_gt_xgraph");

    this->temp_dir_.set_values(this->param_list_, "io",
            "temp_dir",
            "Path to an existing directory where temporary files can be created and then deleted",
            "/vision/scratch/firat/vox", "/vision/scratch/firat/vox");

    dbsks_train_xshock_geom_model_process pro1;
    vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++)
    {
        bpro1_param* par = pars[i];
        vcl_string par_name = par->name();
        if(par_name != "-xgraph-folder" && par_name != "-xgraph-list-file" && par_name != "-output-file")
        {
            param_list_.push_back(
                    convert_parameter_from_bpro1("geom",
                            "[geom] ",
                            par));
        }
    }

    // Output directory (if not object folder)
    this->output_geom_dir_.set_values(this->param_list_, "io",
            "output_geom_dir",
            "output xgraph geometry model directory", "",
            "/vision/scratch/firat/vox_pyramids/horses_geometry_model",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "xgraph_geom_model_directory",
            dborl_parameter_type_info::FILEASSOC);
}
