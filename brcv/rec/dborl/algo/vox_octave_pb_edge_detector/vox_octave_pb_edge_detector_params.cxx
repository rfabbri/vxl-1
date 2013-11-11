// This is /lemsvxl/brcv/rec/dborl/algo/vox_octave_pb_edge_detector/vox_octave_pb_edge_detector_params.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Feb 26, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "vox_octave_pb_edge_detector_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_octave_pb_edge_detector_params::vox_octave_pb_edge_detector_params(vcl_string algo_name) : dborl_algo_params(algo_name)
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
            "-io: input image extension", ".png", ".png");

    //: extension of the edgemap file
    this->edgemap_extension_.set_values(this->param_list_, "io", "edgemap_extensionn",
            "-io: edge map extesion (.edg)", ".edg", ".edg");

    // Output directory (if not object folder)
    this->output_edgemap_dir_.set_values(this->param_list_, "io",
            "output_edgemap_dir",
            "output edgemap directory", "",
            "/vision/scratch/firat/vox_pyramids/horse001_edges",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "edge_map_directory",
            dborl_parameter_type_info::FILEASSOC);

    // Input pyramid folder
    this->input_pyramid_dir_.set_values(this->param_list_, "io",
            "input_pyramid_dir",
            "Image pyramid directory", "",
            "/vision/scratch/firat/vox_pyramids/horse001",
            0, // associated to 0th input object
            dborl_parameter_system_info::NOT_DEFINED,
            "pyramid_directory",
            dborl_parameter_type_info::FILEASSOC);

    this->use_pyramid_.set_values(this->param_list_, "io",
            "use_pyramid",
            "Use image pyramid instead of original image",
            true, true);

    this->min_height_.set_values(this->param_list_, "data",
            "min_height",
            "Minimum height of image to compute edges",
            32, 32);

    this->min_width_.set_values(this->param_list_, "data",
            "min_width",
            "Minimum width of image to compute edges",
            32, 32);

    this->max_height_.set_values(this->param_list_, "data",
            "max_height",
            "Maximum height of image to compute edges",
            250, 250);

    this->max_width_.set_values(this->param_list_, "data",
            "max_width",
            "Maximum width of image to compute edges",
            250, 250);

    dbdet_pb_edge_detector_process pro1;
    vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++)
    {
        param_list_.push_back(
                convert_parameter_from_bpro1("PB",
                        "[PB] ",
                        pars[i]));
    }

}
