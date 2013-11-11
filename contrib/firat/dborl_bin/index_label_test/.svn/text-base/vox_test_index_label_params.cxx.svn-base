// This is /lemsvxl/contrib/firat/dborl_bin/index_label_test/vox_test_index_label_params.cxx

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
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_test_index_label_params::vox_test_index_label_params(vcl_string algo_name) : dborl_algo_params(algo_name)
{

    //: Name of input object
    this->input_object_name_.set_values(
            this->param_list_,
            "io", "input_object_name",
            "input_object_name", "", "",
            0, // for 0th input object
            dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    // Index file for shock match computation
    this->index_filename_.set_values(param_list_,
            "io", "index_filename",
            "path of the index file", "", "",
            0,
            dborl_parameter_system_info::NOT_DEFINED,
            "flat_image",
            dborl_parameter_type_info::FILEASSOC);

    // Index file for shock match computation
    this->assoc_label_.set_values(param_list_,
            "io", "assoc_label",
            "assoc label", "", "",
            0,
            dborl_parameter_system_info::NOT_DEFINED,
            "groundtruth_xgraph_directory",
            dborl_parameter_type_info::FILEASSOC);
}

