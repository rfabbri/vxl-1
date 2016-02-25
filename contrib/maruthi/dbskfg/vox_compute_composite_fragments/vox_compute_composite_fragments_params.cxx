//:
// \file



#include "vox_compute_composite_fragments_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>
#include <dbsk2d/pro/dbsk2d_compute_containment_graph_process.cxx>

//: Constructor
vox_compute_composite_fragments_params::
vox_compute_composite_fragments_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name),
    tag_compute_composite_fragments_("Compute_Composite_Fragments")
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
       ".cem", ".cem");

  //: extension of the image for composite graph fragment extraction
  this->input_image_extension_.set_values
      (this->param_list_, 
       "io", 
       "input_image_extension", 
       "-io: input image extension ", 
       ".jpg", ".jpg");

  // Output composite graph fragment folder (if not object folder)
  this->output_cgraph_fragments_folder_.
      set_values(this->param_list_, "io", 
                 "output_cgraph_fragments_folder", 
                 "output folder to write composite graph fragment storage", "",
                 "/vision/projects/kimia/categorization/output",
                 0, // associated to 0th input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "cgraph_fragment_storage",
                 dborl_parameter_type_info::FILEASSOC);


  //: training path directory
  this->training_path_.set_values
      (this->param_list_, 
       "io", 
       "training_path", 
       "-io: input path to training data ", 
       "/home/mn/Desktop/training", "/home/mn/Desktop/training");

  //: texton path directory
  this->texton_path_.set_values
      (this->param_list_, 
       "io", 
       "texton_path", 
       "-io: input path to texton data ", 
       "/home/mn/Desktop/texton", "/home/mn/Desktop/texton");

  //: First Coefficient of logistic function 
  this->logistic_beta0_.set_values
      (this->param_list_, 
       "cost_params", 
       "logistic_beta0", 
       "[cost params] Coefficient for Logistic Function, Determines Slope", 
       0.2, 0.2);

  //: Second Coefficient of logistic function 
  this->logistic_beta1_.set_values
      (this->param_list_, 
       "cost_params", 
       "logistic_beta1", 
       "[cost params] Coefficient for Logistic Function, Determines Intercept", 
       0.0263, 0.0263);

  // add the parameters of the extract shock patches process
  dbsk2d_compute_containment_graph_process pro1;
  vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      this->param_list_.push_back(convert_parameter_from_bpro1
                                  (tag_compute_composite_fragments_,
                                   "[" + tag_compute_composite_fragments_ + "]",
                                   pars[i]));
  }

 
}

