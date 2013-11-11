//:
// \file



#include "vox_extract_shock_patches_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>
#include <dbskr/pro/dbskr_extract_subgraph_and_find_shock_patches_process.h>

//: Constructor
vox_extract_shock_patches_params::
vox_extract_shock_patches_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name),
    tag_extract_shock_patches_("Extract_Shock_Patches")
{ 

  // Save result to the object folder?
  this->save_to_object_folder_.set_values(this->param_list_, "io", 
    "save_to_object_folder", "-io: save result to object folder ?", 
                                          false, false);

  //: Name of input object
  this->input_object_name_.
      set_values(this->param_list_, 
                 "io", "input_object_name",
                 "input_object_name", "dummy", "dymmy",
                 0, // for 0th input object
                 dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input object
  this->input_object_dir_.
      set_values(this->param_list_, 
                 "io", "input_object_dir",
                 "input object folder", "", 
                 "/vision/images/misc/object",
                 0, // for 0th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);

  // Extension for input association file
  this->input_assoc_dir_.set_values(param_list_, 
                                     "io", "input_assoc_dir", 
                                     "path of the assoc filename", "", "", 
                                     0, // for 0th input object
                                     dborl_parameter_system_info::NOT_DEFINED, 
                                     "extrinsic_shock_graph", 
                                     dborl_parameter_type_info::FILEASSOC);
 
  //: extension of the input shock file
  this->input_shock_extension_.set_values
      (this->param_list_, 
       "io", 
       "input_shock_extension", 
       "-io: input shock extension (.esf) ", 
       ".esf", ".esf");

  //: extension of the image for shock patch extraction
  this->input_image_extension_.set_values
      (this->param_list_, 
       "io", 
       "input_image_extension", 
       "-io: input image extension ", 
       ".png", ".png");

  // Output shock folder (if not object folder)
  this->output_shock_patch_folder_.
      set_values(this->param_list_, "io", 
                 "output_shock_patch_folder", 
                 "output folder to write shock patch storage", "",
                 "/vision/projects/kimia/categorization/output",
                 0, // associated to 0th input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "shock_patch_storage",
                 dborl_parameter_type_info::FILEASSOC);



  // add the parameters of the extract shock patches process
  dbskr_extract_subgraph_and_find_shock_patches_process pro1;
  vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      this->param_list_.push_back(convert_parameter_from_bpro1
                                  (tag_extract_shock_patches_,
                                   "[" + tag_extract_shock_patches_ + "]",
                                   pars[i]));
  }

 
}

