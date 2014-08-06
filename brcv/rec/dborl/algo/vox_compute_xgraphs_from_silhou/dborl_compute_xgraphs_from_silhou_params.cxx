//:
// \file


#include "dborl_compute_xgraphs_from_silhou_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_prune_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_sample_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_gap_transform_process.h>


//#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
//#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
//#include <dbdet/pro/dbdet_sel_process.h>
//#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
//#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
//#include <dbdet/pro/dbdet_prune_fragments_Logistic_Regression.h>

//: Constructor
dborl_compute_xgraphs_from_silhou_params::
dborl_compute_xgraphs_from_silhou_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name),
    tag_contour_tracing_("Contour_Tracer"),
    tag_compute_ishock_("Compute_Ishock"),
//    tag_gap_transform_("Gap_Transform"),
	tag_prune_ishock_("Prune_Ishock"),
    tag_sample_shock_("Sample_Shock"),
    tag_esf_2_xgraph_("Convert_esf_to_xgraph")
{ 
/*
  //: Save the result intrinsic shock?
  this->save_edges_.set_values(this->param_list_, "io", "save_edges", 
    "-io: save edge detection result ?", false, false);

  //: Save the curvlets, intermediate step of symbolic edge linking
  this->save_curvelets_.set_values(this->param_list_, "io", "save_curvelets", 
    "-io: save curvelets ?", false, false);
*/
  // Save result to the object folder?
  this->save_to_object_folder_.
      set_values(this->param_list_, 
                 "io", 
                 "save_to_object_folder", 
                 "-io: save result to object folder ?", true, true);

  //: Name of input object
  this->input_object_name_.set_values(
      this->param_list_, 
      "io", "input_object_name",
      "input_object_name", "", "",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

  //: passes the folder of the input object
  this->input_object_dir_.set_values(this->param_list_, 
                                     "io", "input_object_dir",
                                     "input object folder", "", 
                                     "",
                                     0, // for 0th input object
                                     dborl_parameter_system_info::
                                     INPUT_OBJECT_DIR);

  //: extension of the image file
  this->input_extension_.set_values(this->param_list_, "io", "input_extension", 
    "-io: input image extension", ".jpg", ".jpg");

  // perform contour tracing
  this->trace_contours_.
      set_values(this->param_list_, "io", 
                 "trace_contours", 
                 "-io: trace contours (should be a black/white image) ?", 
                 true, true);
/*
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
       ".cemv", ".cemv");*/

  //: extension of output file
  this->output_extension_.set_values(
      this->param_list_, "io", "output_extension",
      "-io: output extension of sampled shock", ".esf", ".esf");

  // Output shock folder (if not object folder)
  this->output_shock_folder_.
      set_values(this->param_list_, "io", 
                 "output_shock_folder", 
                 "output folder to write computed shock graph", "",
                 "",
                 0, // associated to 0th input object
                 dborl_parameter_system_info::OUTPUT_FILE,
                 "extrinsic_shock_graph",
                 dborl_parameter_type_info::FILEASSOC);

  // Number of iterations to perform to get valid shocks 
  this->num_iter_.set_values(this->param_list_, "io", 
    "num_iter", "-io: number of iterations for valid shock computation ? ", 
                                  5, 5);

  // Perform gap transform on ishock graph? 
//  this->gap_transform_.set_values(this->param_list_, "io", 
//   "gap_transform", "-io: perform gap transform on ishock ? ", true, true);

  // Perform prune ishock on ishock graph? 
  this->prune_ishock_.set_values(this->param_list_, "io", 
    "prune_ishock", "-io: perform prune ishock on ishock ? ", true, true);


  //: extension of the input boundary file
/*  this->input_image_extension_.set_values
      (this->param_list_, 
       "io", "input_image_extention", 
       " -io: image for gap transform ( ignore if not doing gap transform )", 
       ".jpg", 
       ".jpg");
*/
  // Add a bbox for shock computation?
  this->add_bbox_.set_values(this->param_list_, "io", 
    "add_bbox", "-io: add bbox for ishock computation", false, false);


  // save esf file
  this->save_esf_.set_values(this->param_list_, "io", 
    "save_esf", "-io: save .esf file in the middle of the process", false, false);


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



  //: add the parameter for contour tracing
  dbdet_contour_tracer_process pro1;
  vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
//  pars = pro6.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      param_list_.push_back(
          convert_parameter_from_bpro1(tag_contour_tracing_, 
                                       "[" + tag_contour_tracing_+ "] ",
                                       pars[i]));
  }

  // add the parameters of the intrinsinc shock process
  dbsk2d_compute_ishock_process pro2;
  pars = pro2.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      this->param_list_.push_back(convert_parameter_from_bpro1
                                  (tag_compute_ishock_,
                                   "[" + tag_compute_ishock_ + "]",
                                   pars[i]));
  }

  // add the parameters of prune shock process
  dbsk2d_prune_ishock_process pro3;
  pars = pro3.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      this->param_list_.push_back(convert_parameter_from_bpro1
                                  (tag_prune_ishock_,
                                   "[" + tag_prune_ishock_ + "]",
                                   pars[i]));
  }

/*
  // add the parameters of the gap transform
  dbsk2d_gap_transform_process pro3;
  pars = pro3.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      this->param_list_.push_back(convert_parameter_from_bpro1
                                  (tag_gap_transform_, 
                                   "[" + tag_gap_transform_ + "]",
                                   pars[i]));
  }
*/
  // add the parameters of the sampling of the intrinsinc shock
  dbsk2d_sample_ishock_process pro4;
  pars = pro4.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      this->param_list_.push_back(convert_parameter_from_bpro1
                                  (tag_sample_shock_, 
                                   "[" + tag_sample_shock_ + "]",
                                   pars[i]));
  }
}

