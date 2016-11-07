// This is brcv/rec/dborl/algo/vox_octave_create_image_pyramid/vox_octave_create_image_pyramid_params.cxx

//:
// \file
// \brief parameter set
//
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Feb 24, 2010
//


#include "vox_octave_create_image_pyramid_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_octave_create_image_pyramid_params::vox_octave_create_image_pyramid_params(vcl_string algo_name) : dborl_algo_params(algo_name)
{

    //: Name of input object
    this->input_object_name_.set_values(
            this->param_list_,
            "io", "input_object_name",
            "input_object_name", "horse001", "horse001",
            0, // for 0th input object
            dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    //: passes the folder of the input object
    this->input_object_dir_.set_values(this->param_list_,
            "io", "input_object_dir",
            "input object folder", "",
            "/vision/projects/kimia/categorization/vox/weizmann_gray/horse001",
            0, // for 0th input object
            dborl_parameter_system_info::
            INPUT_OBJECT_DIR);

    //: extension of the image file
    this->input_extension_.set_values(this->param_list_, "io", "input_extention",
            "-io: input image extension", ".jpg", ".jpg");

    //: extension of output file
    this->output_extension_.
    set_values(this->param_list_, "io",
            "pyramid_image_extension",
            "-io: pyramid image extension"
            , ".png", ".png");

    // Output folder (if not object folder)
    this->output_pyramid_folder_.set_values(this->param_list_, "io",
            "output_pyramid_folder",
            "output folder to write pyramid images", "",
            "/vision/scratch/firat/vox_pyramids/horse001",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "pyramid_directory",
            dborl_parameter_type_info::FILEASSOC);

    this->interp_type_.set_values(this->param_list_, "data",
            "interp_type",
            "interpolation type",
            "nearest", "nearest");

    this->step_base_.set_values(this->param_list_, "data",
            "step_base",
            "step base",
            2, 2);

    this->step_power_.set_values(this->param_list_, "data",
            "step_power",
            "step power",
            0.25, 0.25);

    this->num_steps_.set_values(this->param_list_, "data",
            "num_steps",
            "number of steps",
            20, 20);

}

