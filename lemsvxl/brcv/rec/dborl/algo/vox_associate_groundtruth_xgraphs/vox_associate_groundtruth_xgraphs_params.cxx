// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_associate_groundtruth_xgraphs/vox_associate_groundtruth_xgraphs_params.cxx

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
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_associate_groundtruth_xgraphs_params::vox_associate_groundtruth_xgraphs_params(vcl_string algo_name) : dborl_algo_params(algo_name)
{
    //: Name of input object
    this->input_object_name_.set_values(
            this->param_list_,
            "io", "input_object_name",
            "input_object_name", "horse001", "horse001",
            0, // for 0th input object
            dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    // Output directory (if not object folder)
    this->output_xgraph_dir_.set_values(this->param_list_, "io",
            "output_xgraph_dir",
            "output xgraph directory", "",
            "/vision/scratch/firat/vox_pyramids/horse001_xgraph",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "groundtruth_xgraph_directory",
            dborl_parameter_type_info::FILEASSOC);

    this->input_xgraph_list_file_.set_values(this->param_list_, "io",
            "input_xgraph_list_file",
            "Path to the input file containing the list of groundtruth xgraphs",
            "/vision/scratch/firat/weizmann_xshock/gt.txt", "/vision/scratch/firat/weizmann_xshock/gt.txt");
}
