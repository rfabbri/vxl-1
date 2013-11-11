//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 29, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "vox_convert_and_associate_kovesi_output_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_convert_and_associate_kovesi_output_params::vox_convert_and_associate_kovesi_output_params(vcl_string algo_name) : dborl_algo_params(algo_name)
{
    //: Name of input object
    this->input_object_name_.set_values(
            this->param_list_,
            "io", "input_object_name",
            "input_object_name", "applelogos_box", "applelogos_box",
            0, // for 0th input object
            dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    // Output directory (if not object folder)
    this->output_linked_edge_dir_.set_values(this->param_list_, "io",
            "output_linked_edge_dir",
            "output linked edge directory", "",
            "/vision/scratch/firat/ethz/applelogos_box_linked_edges",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "linked_edge_map_directory",
            dborl_parameter_type_info::FILEASSOC);

    this->input_kovesi_folder_list_file_.set_values(this->param_list_, "io",
            "input_kovesi_folder_list_file",
            "Path to the input file containing the list of kovesi output folders",
            "/vision/scratch/firat/ethz/kovesi_out_list.txt", "/vision/scratch/firat/ethz/kovesi_out_list.txt");

    this->min_height_.set_values(this->param_list_, "data",
            "min_height",
            "Minimum height of image to compute edges",
            32, 32);

    this->min_width_.set_values(this->param_list_, "data",
            "min_width",
            "Minimum width of image to compute edges",
            32, 32);

    // extension of the input edgemap
    this->input_cem_extension_.set_values(this->param_list_, "io", "input_cem_extension",
            "[DATA] input cem file extension",
            "_pb_linked.cem",  //"_edges.tif"
            "_pb_linked.cem" //"_edges.tif"
    );

    // extension of the input edgemap
    this->input_edgeimg_extension_.set_values(this->param_list_, "io", "input_edgeimg_extension",
            "[DATA] input edgemap image file extension",
            "_pb_edges.png",  //"_edges.tif"
            "_pb_edges.png" //"_edges.tif"
    );

    // extension of the input edgemap
    this->output_cem_extension_.set_values(this->param_list_, "io", "output_cem_extension",
            "[DATA] output cem file extension",
            ".cem",  //"_edges.tif"
            ".cem" //"_edges.tif"
    );

    //: extension of the edgemap file
    this->output_edgemap_extension_.set_values(this->param_list_, "io", "output_edgemap_extension",
            "-io: output edge map extesion (.edg)", "_linked.edg", "_linked.edg");
}
