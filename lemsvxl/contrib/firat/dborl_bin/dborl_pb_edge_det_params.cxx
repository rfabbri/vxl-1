#include "dborl_pb_edge_det_params.h"

dborl_pb_edge_det_params::dborl_pb_edge_det_params (vcl_string algo_name) : dborl_algo_params(algo_name) 
{ 
        algo_abbreviation_ = "pb_edge_det";
        
        input_dir_.set_values (param_list_, "io", "input_dir", "input object", "", 
      "/vision/scratch/firat/weizmann_original", //default directory
      0,   // for 1th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR);

        input_name_.set_values (param_list_, "io", "input_name", "Input object name", "horse001.jpg", "horse001.jpg", 
        0,   // for 1th input object
        dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

        output_dir_.set_values(param_list_,  "io",  "output_folder",  "output folder to store results", "/vision/scratch/firat/pb_output/", 
        "/vision/scratch/firat/pb_output/", 
        0,   // associate to 0th input object
        dborl_parameter_system_info::OUTPUT_FILE, "shock_matcher_result", dborl_parameter_type_info::FILEASSOC);

        index_filename_.set_values(param_list_, "io_data", "index_filename", "path of the index file", "", "", 1, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC);

}
