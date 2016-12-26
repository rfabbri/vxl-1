//:
// \file



#include "vox_compute_ishock_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_sample_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_gap_transform_process.h>

//: Constructor
vox_compute_ishock_params::
vox_compute_ishock_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name),
    tag_compute_ishock_("Compute_Ishock"),
    tag_gap_transform_("Gap_Transform"),
    tag_sample_shock_("Sample_Shock")
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
                                     "contour_map", 
                                     dborl_parameter_type_info::FILEASSOC);
 
  //: extension of the input boundary file
  this->input_contour_extension_.set_values
      (this->param_list_, 
       "io", 
       "input_contour_extention", 
       "-io: input contour extension (.cem,.cemv,.con) ", 
       ".cemv", ".cemv");

  //: extension of output file
  this->output_extension_.set_values(
      this->param_list_, "io", "output_extension",
      "-io: output extension of sampled shock", ".esf", ".esf");

  // Output shock folder (if not object folder)
  this->output_shock_folder_.
      set_values(this->param_list_, "io", 
                 "output_shock_folder", 
                 "output folder to write computed shock graph", "",
                 "/vision/projects/kimia/categorization/output",
                 0, // associated to 0th input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "extrinsic_shock_graph",
                 dborl_parameter_type_info::FILEASSOC);

  // Number of iterations to perform to get valid shocks 
  this->num_iter_.set_values(this->param_list_, "io", 
    "num_iter", "-io: number of iterations for valid shock computation ? ", 
                                  5, 5);

  // Perform gap transform on ishock graph? 
  this->gap_transform_.set_values(this->param_list_, "io", 
    "gap_transform", "-io: perform gap transform on ishock ? ", true, true);

  //: extension of the input boundary file
  this->input_image_extension_.set_values
      (this->param_list_, 
       "io", "input_image_extention", 
       " -io: image for gap transform ( ignore if not doing gap transform )", 
       ".jpg", 
       ".jpg");

  // Add a bbox for shock computation?
  this->add_bbox_.set_values(this->param_list_, "io", 
    "add_bbox", "-io: add bbox for ishock computation", true, true);

  // add the parameters of the intrinsinc shock process
  dbsk2d_compute_ishock_process pro1;
  vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      this->param_list_.push_back(convert_parameter_from_bpro1
                                  (tag_compute_ishock_,
                                   "[" + tag_compute_ishock_ + "]",
                                   pars[i]));
  }


  // add the parameters of the gap transform
  dbsk2d_gap_transform_process pro2;
  pars = pro2.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      this->param_list_.push_back(convert_parameter_from_bpro1
                                  (tag_gap_transform_, 
                                   "[" + tag_gap_transform_ + "]",
                                   pars[i]));
  }

  // add the parameters of the sampling of the intrinsinc shock
  dbsk2d_sample_ishock_process pro3;
  pars = pro3.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      this->param_list_.push_back(convert_parameter_from_bpro1
                                  (tag_sample_shock_, 
                                   "[" + tag_sample_shock_ + "]",
                                   pars[i]));
  }

 
}

