// This is /lemsvxl/brcv/rec/dborl/algo/vox_octave_kovesi_edge_linker/vox_octave_kovesi_edge_linker_params.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 2, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "vox_octave_kovesi_edge_linker_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

//: Constructor
vox_octave_kovesi_edge_linker_params::vox_octave_kovesi_edge_linker_params(vcl_string algo_name) : dborl_algo_params(algo_name)
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

    //: extension of the edgemap file
    this->input_edgemap_extension_.set_values(this->param_list_, "io", "input_edgemap_extension",
            "-io: input edge map extesion (.edg)", ".edg", ".edg");

    //: extension of the edgemap file
    this->output_edgemap_extension_.set_values(this->param_list_, "io", "output_edgemap_extension",
            "-io: output edge map extesion (_linked.edg)", "_linked.edg", "_linked.edg");

    // Output directory (if not object folder)
    this->output_linked_edge_dir_.set_values(this->param_list_, "io",
            "output_linked_edge_dir",
            "output linked edge directory", "",
            "/vision/scratch/firat/vox_pyramids/horse001_linked_edges",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "linked_edge_map_directory",
            dborl_parameter_type_info::FILEASSOC);

    // Input edgemap folder
    this->input_edgemap_dir_.set_values(this->param_list_, "io",
            "input_edgemap_dir",
            "Input edgemap directory", "",
            "/vision/scratch/firat/vox_pyramids/horse001_edges",
            0, // associated to 0th input object
            dborl_parameter_system_info::NOT_DEFINED,
            "edge_map_directory",
            dborl_parameter_type_info::FILEASSOC);


    this->output_cemv_extension_.set_values(this->param_list_, "io",
            "output_cemv_extension",
            "Output cemv extension (.cem, .cemv)",
            ".cem", ".cem");

    this->use_pyramid_.set_values(this->param_list_, "io",
            "use_pyramid",
            "Use image pyramid instead of original image",
            true, true);

    dbdet_kovesi_edge_linker_process pro1;
    vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++)
    {
        param_list_.push_back(
                convert_parameter_from_bpro1("kovesi",
                        "[kovesi] ",
                        pars[i]));
    }

}
