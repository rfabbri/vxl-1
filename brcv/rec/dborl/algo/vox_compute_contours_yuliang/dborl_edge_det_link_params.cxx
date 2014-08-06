//:
// \file


#include "dborl_edge_det_link_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>


#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbdet/pro/dbdet_generic_linker_process.h>
#include <dbdet/pro/dbdet_prune_fragments_Logistic_Regression.h>

//: Constructor
dborl_edge_det_link_params::
dborl_edge_det_link_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name),
    tag_gray_edge_detection_("Edge_Detection_Gray"),
    tag_color_edge_detection_("Edge_Detection_Color"),
    tag_edge_linking_("Edge_Linking"),
    tag_gen_linking_("GEN_Linking"),
    tag_extract_contours_("Extract_Contours"),
    tag_prune_contours_("Prune_Contours"),
    tag_contour_tracing_("Contour_Tracer"),
    tag_convert_edgemap_to_image_("Convert_Edgemap_To_Image"),
	tag_prune_contours_logistic_("Prune_Contours_Logistic")
{ 

  //: Save the result intrinsic shock?
  this->save_edges_.set_values(this->param_list_, "io", "save_edges", 
    "-io: save edge detection result ?", false, false);

  //: Save the curvlets, intermediate step of symbolic edge linking
  this->save_curvelets_.set_values(this->param_list_, "io", "save_curvelets", 
    "-io: save curvelets ?", false, false);

  this->save_edges_inds_.set_values(this->param_list_, "io", "save_edges_inds_", 
    "-io: save edge indicators ?", false, false);

  // Save result to the object folder?
  this->save_to_object_folder_.
      set_values(this->param_list_, 
                 "io", 
                 "save_to_object_folder", 
                 "-io: save result to object folder ?", false, false);

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


  //: extension of the edge map file
  this->edge_extension_.set_values(this->param_list_, "io", "edge_extension", 
    "-io: input edge map extension", ".edg", ".edg");

  //: extension of output file
  this->output_extension_.
      set_values(this->param_list_, "io", 
                 "output_extension",
                 "-io: output extension of contours (.con,.cem,.cemv) "
                 , ".cem", ".cem");

  //: extension of edge file
  this->edge_extension_.
      set_values(this->param_list_, "io", 
                 "edge_extension",
                 "-io: edge extension of edges (.edg) "
                 , ".edg", ".edg");

  // Output shock folder (if not object folder)
  this->output_edge_link_folder_.set_values(this->param_list_, "io", 
    "output_edge_link_folder", 
    "output folder to write computed contour map", "",
    "/vision/projects/kimia/categorization/output",
    0, // associated to 0th input object
    dborl_parameter_system_info::OUTPUT_FILE,
    "contour_map",
    dborl_parameter_type_info::FILEASSOC);

  // use existing edges?
  this->use_existing_edges_.
      set_values(this->param_list_, "io", 
                 "use_existing_edges", 
                 "-io: use an existing edge file ?"
                 , false, false);

  // use existing cem?
  this->use_existing_cem_.
      set_values(this->param_list_, "io", 
                 "use_existing_cem", 
                 "-io: use an existing cem file ?"
                 , false, false);

  // prune curve fragments ?
  this->prune_contours_.
      set_values(this->param_list_, "io", 
                 "prune_contours", 
                 "-io: prune contorus based on contrast ?"
                 , false, false);

  // perform contour tracing
  this->trace_contours_.
      set_values(this->param_list_, "io", 
                 "trace_contours", 
                 "-io: trace contours (should be a black/white image) ?", 
                 false, false);

  // perform contour tracing
  this->edge_linking_method_.
      set_values(this->param_list_, "io", 
                 "edge_linking_method", 
                 "-io: edge_linking_method (should be sel/gen) ?", 
                 "sel", "sel");

  // convert to bianry map ?
  this->convert_edgemap_to_image_.
      set_values(this->param_list_, "io", 
                 "convert_edgemap_to_image", 
                 "-io: convert_edgemap_to_image ?"
                 , false, false);

  // prune using logistic ?
  this->prune_contours_logistic_.
      set_values(this->param_list_, "io", 
                 "prune_contours_logistic", 
                 "-io: prune contorus using parameters from logistic regression ?"
                 , false, false);
  //: add the parameters of the dbdet_third_order_edge_detector_process
  dbdet_third_order_edge_detector_process pro1;
  vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      param_list_.push_back(
          convert_parameter_from_bpro1(tag_gray_edge_detection_,
                                       "[" + tag_gray_edge_detection_+ "] ",
                                       pars[i]));
  }

  //: add the parameters for the dbdet_third_order_color_edge_detector_process
  dbdet_third_order_color_edge_detector_process pro2;
  pars = pro2.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      param_list_.push_back(
          convert_parameter_from_bpro1(tag_color_edge_detection_, 
                                       "[" + tag_color_edge_detection_+ "] ",
                                       pars[i]));
  }

  //: add the parameters for symbolic edge linking
  dbdet_sel_process pro3;
  pars = pro3.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      param_list_.push_back(
          convert_parameter_from_bpro1(tag_edge_linking_, 
                                       "[" + tag_edge_linking_ + "] ",
                                       pars[i]));
  }


  //: add the parameters for extract regular contours
  dbdet_sel_extract_contours_process pro4;
  pars = pro4.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      param_list_.push_back(
          convert_parameter_from_bpro1(tag_extract_contours_, 
                                       "[" + tag_extract_contours_+ "] ",
                                       pars[i]));
  }


  //: add the parameter for pruning
  dbdet_prune_curves_process pro5;
  pars = pro5.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      param_list_.push_back(
          convert_parameter_from_bpro1(tag_prune_contours_, 
                                       "[" + tag_prune_contours_+ "] ",
                                       pars[i]));
  }

  //: add the parameter for contour tracing
  dbdet_contour_tracer_process pro6;
  pars = pro6.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      param_list_.push_back(
          convert_parameter_from_bpro1(tag_contour_tracing_, 
                                       "[" + tag_contour_tracing_+ "] ",
                                       pars[i]));
  }

  //: add parameters for genetic edge linking, added by Yuliang
  dbdet_generic_linker_process pro7;
  pars = pro7.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      param_list_.push_back(
          convert_parameter_from_bpro1(tag_gen_linking_, 
                                       "[" + tag_gen_linking_ + "] ",
                                       pars[i]));
  }  

  //: add the parameter for convert to binary map
  dbdet_convert_edgemap_to_image_process pro8;
  pars = pro8.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      param_list_.push_back(
          convert_parameter_from_bpro1(tag_convert_edgemap_to_image_, 
                                       "[" + tag_convert_edgemap_to_image_+ "] ",
                                       pars[i]));
  }

  //: add the parameter for pruning logistic
  dbdet_prune_fragments_Logistic_Regression pro9;
  pars = pro9.parameters()->get_param_list();
  for (unsigned i = 0; i < pars.size(); i++) 
  {
      param_list_.push_back(
          convert_parameter_from_bpro1(tag_prune_contours_logistic_, 
                                       "[" + tag_prune_contours_logistic_+ "] ",
                                       pars[i]));
  }
}

