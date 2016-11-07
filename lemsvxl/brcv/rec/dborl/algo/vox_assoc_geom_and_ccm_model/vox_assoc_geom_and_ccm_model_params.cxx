// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_assoc_geom_and_ccm_model/vox_assoc_geom_and_ccm_model_params.cxx

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
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_assoc_geom_and_ccm_model_params::vox_assoc_geom_and_ccm_model_params(vcl_string algo_name) : dborl_algo_params(algo_name)
{
    // Output directory (if not object folder)
    this->output_ccm_dir_.set_values(this->param_list_, "io",
            "output_ccm_dir",
            "associated xgraph ccm model directory", "",
            "/vision/scratch/firat/ethz/ccm-model",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "xgraph_ccm_model_directory",
            dborl_parameter_type_info::FILEASSOC);

    this->output_geom_dir_.set_values(this->param_list_, "io",
            "output_geom_dir",
            "associated xgraph geometry model directory", "",
            "/vision/scratch/firat/ethz/geom-model",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "xgraph_geom_model_directory",
            dborl_parameter_type_info::FILEASSOC);

    this->input_ccm_.set_values(this->param_list_, "io",
            "input_ccm",
            "Path to the original ccm file",
            "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/giraffes-xgraph/giraffes-xgraph_ccm_model-v2-2009aug31-chamfer_4.xml",
            "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/giraffes-xgraph/giraffes-xgraph_ccm_model-v2-2009aug31-chamfer_4.xml");

    this->input_geom_.set_values(this->param_list_, "io",
            "input_geom",
            "Path to the original geom file",
            "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/giraffes-xgraph/giraffes-xgraph_geom_model-v2-2009sep02.xml",
            "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/giraffes-xgraph/giraffes-xgraph_geom_model-v2-2009sep02.xml");
}
