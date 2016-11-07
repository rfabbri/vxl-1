// This is /lemsvxl/brcv/rec/dborl/algo/vox_convert_edg_to_img_and_orient/vox_convert_edg_to_img_and_orient_params.cxx

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

#include "vox_convert_edg_to_img_and_orient_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_convert_edg_to_img_and_orient_params::vox_convert_edg_to_img_and_orient_params(vcl_string algo_name) : dborl_algo_params(algo_name)
{

    //: Name of input object
    this->input_object_name_.set_values(
            this->param_list_,
            "io", "input_object_name",
            "input_object_name", "horse001", "horse001",
            0, // for 0th input object
            dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    this->input_edgemap_dir_.set_values(this->param_list_, "io",
            "input_edgemap_dir",
            "Input edgemap directory", "",
            "/vision/scratch/firat/vox_pyramids/horse001_edges",
            0, // associated to 0th input object
            dborl_parameter_system_info::NOT_DEFINED,
            "edge_map_directory",
            dborl_parameter_type_info::FILEASSOC);

    this->input_linked_edgemap_dir_.set_values(this->param_list_, "io",
            "input_linked_edgemap_dir",
            "Input linked edgemap directory", "",
            "/vision/scratch/firat/vox_pyramids/horse001_linked_edges",
            0, // associated to 0th input object
            dborl_parameter_system_info::NOT_DEFINED,
            "linked_edge_map_directory",
            dborl_parameter_type_info::FILEASSOC);


    this->input_edgemap_extension_.set_values(this->param_list_, "io", "input_edgemap_extension",
            "-io: input edge map extesion (.edg)", ".edg", ".edg");

    this->input_linked_edgemap_extension_.set_values(this->param_list_, "io", "input_linked_edgemap_extension",
            "-io: input linked edge map extesion (_linked.edg)", "_linked.edg", "_linked.edg");

    this->use_pyramid_.set_values(this->param_list_, "io",
            "use_pyramid",
            "Use image pyramid instead of original image",
            true, true);

    this->use_linked_.set_values(this->param_list_, "io",
            "use_linked",
            "Use linked edgemap (on) / Use raw edgemap (off)",
            true, true);

    this->is_gray_.set_values(this->param_list_, "data",
            "is_gray",
            "Gray level edge map image (on) / Binary edge map image (off)",
            false, false);

    this->output_edge_image_extension_.set_values(this->param_list_, "io", "output_edge_image_extension",
            "-io: output edge map image extension", ".png", ".png");

    this->output_edge_orient_extension_.set_values(this->param_list_, "io", "output_edge_orient_extension",
            "-io: output edge map orientation extension", "_orient.txt", "_orient.txt");

    this->output_edge_image_orient_dir_.set_values(this->param_list_, "io",
            "output_edge_image_orient_dir",
            "Output edge map image and orientation directory", "",
            "/vision/scratch/firat/vox_pyramids/horse001_edge_im_orient",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "edge_map_image_and_oriention_directory",
            dborl_parameter_type_info::FILEASSOC);
}
