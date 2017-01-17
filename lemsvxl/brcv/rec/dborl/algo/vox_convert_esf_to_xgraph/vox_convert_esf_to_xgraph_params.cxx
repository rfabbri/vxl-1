// This is rec/dborl/algo/vox_convert_esf_to_xgraph/vox_convert_esf_to_xgraph_params.cxx

//:
// \file



#include "vox_convert_esf_to_xgraph_params.h"
#include <dborl/algo/dborl_algo_params.h>

//: Constructor
vox_convert_esf_to_xgraph_params::
vox_convert_esf_to_xgraph_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 
  // Process parameters

  //: Name of input model
  this->object_name_.set_values(this->param_list_, 
                 "io", 
                 "object_name",
                 "object_name", 
                 "dog1", 
                 "dog1",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input model
  this->object_dir_.set_values(this->param_list_, 
                 "io", 
                 "object_dir",
                 "object folder", 
                 "dummy", 
                 "V:/projects/kimia/shockshape/learnskeleton/data/vox-kimia-99-dataset",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);


  // Output shock folder for matches
  this->output_folder_.set_values(this->param_list_, 
                 "io", 
                 "output_folder", 
                 "output folder to write xgraph", 
                 "xgraph", // value
                 "D:/vision/docs/learnskeleton-docs/vox-output", // default value
                 0, // associated to 1st input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "xgraph",
                 dborl_parameter_type_info::FILEASSOC);

    // Save result to the object folder?
  this->save_to_object_folder_.set_values(this->param_list_, "io", "save_to_object_folder", 
    "[OUTPUT] save result to object folder?", true, true);


  //: Extension of xgraph file
  this->xgraph_extension_.set_values(this->param_list_, 
    "io", "xgraph_extension", 
    "[OUTPUT] Extension of xgraph file (added to object name)", 
    "-xgraph.xml",
    "-xgraph.xml");


  // Process parameters
  this->convert_sk2d_to_sksp_tol_.set_values(this->param_list_, 
    "io", "convert_sk2d_to_sksp_tol", 
    "[CONVERT] Tolerance for converting sk2d graph to sksp graph (pixels)", 
    0.5f,
    0.5f);

}

