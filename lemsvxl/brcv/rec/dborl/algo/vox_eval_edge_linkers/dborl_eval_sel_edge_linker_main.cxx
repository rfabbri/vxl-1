//:
// \file
// \author Amir Tamrakar
// \date 08/01/08
//
//        An algorithm to evaluate edge detection and linking algorithms on images, 
//        The input object is a single image, 
//        The output is an edge map and a contour fragment map, 
//        with an option to associate to the image or to write to the object folder
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#include "dborl_eval_sel_edge_linker_params.h"
#include "dborl_eval_sel_edge_linker_params_sptr.h"

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>

#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/edge/dbdet_edgemap.h>

#include <dbdet/algo/dbdet_gen_edge_det.h>
#include <dbdet/algo/dbdet_third_order_edge_det.h>
#include <dbdet/algo/dbdet_load_edg.h>

#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/sel/dbdet_sel_utils.h>

#include <dbdet/algo/dbdet_generic_linker.h>
#include <dbdet/algo/dbdet_postprocess_contours.h>
#include <dbdet/algo/dbdet_cem_file_io.h>

#include <dbdet/algo/dbdet_eval_edge_det.h>

int main(int argc, char *argv[]) 
{
  dborl_eval_sel_edge_linker_params_sptr params = new dborl_eval_sel_edge_linker_params("dborl_eval_sel_edge_linker");  // constructs with the default values;
  if (!params->parse_command_line_args(argc, argv)){
    vcl_cout <<"WTF1";
    return 0;
  }

  // always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
    vcl_cout << "problems in writing params file to: " << params->print_params_file() << vcl_endl;

  if (params->exit_with_no_processing() || params->print_params_only()){
    vcl_cout << "WTF2";
    return 0;
  }

  // always call this method to actually parse the input parameter file whose name is extracted from the command line
  if (!params->parse_input_xml()){
    vcl_cout << "WTF3";
    return 0;
  }

  // 1) load the input image
  vcl_string input_img = params->input_object_dir_() + "/" + params->input_object_name_() + params->input_extension_();
  if (!vul_file::exists(input_img)) {
    vcl_cout << "Cannot find image: " << input_img << "\n";
    return 0;
  }

  vil_image_view<vxl_byte> image  = vil_load(input_img.c_str());
  if (!image) {
    vcl_cout << "Cannot load image: " << input_img << "\n";
    return 0;
  }

  vcl_cout << "Got everything now onto business...\n";
  vcl_cout.flush();

  // 2) detect edges
  int det_opt = params->edge_det_type_();        //detector option
  double sigma = params->edge_det_sigma_();      //sigma
  double threshold = params->edge_det_thresh_();  //threshold
  int N = params->edge_det_N_();                 //interpolation depth             

  bool adaptive_thresh = false;                  //adaptive threshold or not
  
  dbdet_edgemap_sptr EM; //the edge map 
  switch(det_opt)
  {
    case 1: 
      EM = dbdet_detect_generic_edges(image, sigma, threshold, N);
      break;
    case 2:
      EM = dbdet_detect_third_order_edges(image, sigma, threshold, N, 0, 0, 0, adaptive_thresh, false, true);
      break;
    default:
      vcl_cout << "invalid edge det type...";
      return 0;
  }

  vcl_cout << "\n Done Edge Detection\n";
  
  // 3) save this edge map onto a file
  if (params->save_edges_()) 
  {
    vcl_string output_file;
    if (params->save_to_object_folder_()) 
      output_file = params->input_object_dir_() + "/";
    else {
      output_file = params->output_dir_() + "/";

      if (!vul_file::exists(output_file)) 
        vul_file::make_directory(output_file);
    }
    
    output_file = output_file + params->input_object_name_() + params->edg_output_extension_();

    if (!dbdet_save_edg(output_file, EM)) {
      vcl_cout << "Problems in saving edge file: " << output_file << vcl_endl;
      return 0;
    }
  }

  vcl_cout << "Now onto Linking...\n ";
  vcl_cout.flush();

  // 4) call the sel linker to link the edge map into contour fragments
  dbdet_curvelet_map CM;          //the curvelet map (intermediate structure)
  dbdet_edgel_link_graph ELG;     //the edgel link graph (intermediate structure)
  dbdet_curve_fragment_graph CFG; //the curve fragment graph

  //The curvelet formation parameters
  bool bCentered_grouping=true, bBidirectional_grouping=false;

  switch(params->cvlet_type_()) //set the grouping flags from the choice of cvlet type
  {
  case 0: //Anchor Centered
    bCentered_grouping = true;
    bBidirectional_grouping = false;
    vcl_cout << "Cvlet type: Anchor centered \n";

    break;
  case 1: //Anchor Centered/Bidirectional
    bCentered_grouping = true;
    bBidirectional_grouping = true;
    vcl_cout << "Cvlet type: Anchor centered bidirectional\n";
    break;
  case 2: //Anchor Leading/Bidirectional
    bCentered_grouping = false;
    bBidirectional_grouping = true;
    vcl_cout << "Cvlet type: Anchor Leading bidirectional \n";
    break;
  case 3: //ENO Style around Anchor
    bCentered_grouping = false;
    bBidirectional_grouping = false;
    vcl_cout << "Cvlet type: ENO style \n";
    break;
  }

  vcl_cout.flush();

  //the params class for sel
  dbdet_curvelet_params cvlet_params(dbdet_curve_model::CC, 
                                     params->nrad_(), params->dt_(), params->dx_(), params->badap_uncer_(), 
                                     params->token_len_(), params->max_k_(), params->max_gamma_(),
                                     bCentered_grouping,
                                     bBidirectional_grouping);

  vcl_cout << "Initialized params...\n";
  vcl_cout.flush();

  //construct the linker
  dbdet_sel_sptr edge_linker;

  switch (params->curve_model_type_())
  {
    case 0: //linear_model
      cvlet_params.C_type = dbdet_curve_model::LINEAR;
      edge_linker = new dbdet_sel<dbdet_linear_curve_model>(EM, CM, ELG, CFG, cvlet_params);
      vcl_cout << "Curve Model: Linear \n";
      break;
    case 1: //CC_model new
      cvlet_params.C_type = dbdet_curve_model::CC2;
      edge_linker = new dbdet_sel<dbdet_CC_curve_model_new>(EM, CM, ELG, CFG, cvlet_params);
      vcl_cout << "Curve Model: Circular Arc 2d \n";
      break;
    case 2: //CC_model 3d bundle
      cvlet_params.C_type = dbdet_curve_model::CC3d;
      edge_linker = new dbdet_sel<dbdet_CC_curve_model_3d>(EM, CM, ELG, CFG, cvlet_params);
      vcl_cout << "Curve Model: Circular Arc 3d \n";
      break;
    case 3: //ES_model
      cvlet_params.C_type = dbdet_curve_model::ES;
      edge_linker = new dbdet_sel<dbdet_ES_curve_model>(EM, CM, ELG, CFG, cvlet_params);
      vcl_cout << "Curve Model: ES \n";
      break;
  }

  vcl_cout << "\n Edge Linker Initialized...\n";
  vcl_cout.flush();

  switch(params->hybrid_method_())
  {
    case 0: //standard SEL
    {
      //Extract the contours
      vcl_cout << "Extracting contours: Standard SEL \n";
      vcl_cout.flush();

      edge_linker->extract_image_contours();
      break;
    }
    case 1: //Hybrid method: first compute contours using GEL, then filter them by computing curvelets
    {
      vcl_cout << "Extracting contours: Hybrid method \n";
      vcl_cout.flush();

      // A) use the generic linker to compute contours first
      vbl_array_2d<bool>  EULM;//required data structures for the edge linker
      dbdet_generic_linker GEL_linker(EM, CFG, EULM, threshold);//construct a generic linker

      //set various flags
      GEL_linker.require_appearance_consistency(false);
      GEL_linker.require_orientation_consistency(false);//orientation consistency
      GEL_linker.require_smooth_continuation(false);//smooth continuation
      GEL_linker.require_low_curvatures(false);//low curvatures

      //Extract the contours
      GEL_linker.extract_image_contours_by_tracing();

      // B) Now compute the Curvelets using SEL
      edge_linker->set_appearance_usage(0); //do not use
      edge_linker->use_hybrid_ = false;
      //edge_linker->form_curvelets_from_contours(max_size_to_group);
      edge_linker->form_curvelets_from_contours();

      // C) construct the link graph
      edge_linker->use_anchored_curvelets_only();
      edge_linker->construct_the_link_graph(4, 0); //use all curvelets to form links

      // D) extract contours from the link graph (this will clear the contours from GEL)
      edge_linker->extract_regular_contours_from_the_link_graph();

      break;
    }
    case 2: //Hybrid method: also compute curvelets between contours
    {
      vcl_cout << "Extracting contours: Hybrid Method + Close gaps \n";
      vcl_cout.flush();

      // A) use the generic linker to compute contours first
      vbl_array_2d<bool>  EULM;//required data structures for the edge linker
      dbdet_generic_linker GEL_linker(EM, CFG, EULM, threshold);//construct a generic linker

      //set various flags
      GEL_linker.require_appearance_consistency(false);
      GEL_linker.require_orientation_consistency(false);//orientation consistency
      GEL_linker.require_smooth_continuation(false);//smooth continuation
      GEL_linker.require_low_curvatures(false);//low curvatures

      //Extract the contours
      GEL_linker.extract_image_contours_by_tracing();

      // B) Now compute the Curvelets using SEL
      edge_linker->set_appearance_usage(0); //do not use
      //edge_linker->form_curvelets_from_contours(max_size_to_group);
      edge_linker->form_curvelets_from_contours();

      // C) compile the edge -> contour mapping
      edge_linker->compile_edge_to_contour_mapping();
      //use this mapping to avoid forming groupings with edgel skips in them
      edge_linker->use_hybrid_ = true;

      //now construct the curvelet map 
      edge_linker->build_curvelets_greedy(unsigned(2*params->nrad_()), false, false);

      // D) construct the link graph
      edge_linker->use_anchored_curvelets_only();
      edge_linker->construct_the_link_graph(4, 0); //use all curvelets to form links

      // E) extract contours from the link graph (this will clear the contours from GEL)
      edge_linker->extract_regular_contours_from_the_link_graph();
      break;
    }
  }

  vcl_cout << "\n Done Edge Linking! \n";
  vcl_cout.flush();

  //prune contours by length (prune the shortest of the contour fragments)
  prune_contours_by_length(CFG, params->len_thresh_());

  // 5) save the linked contour files
  if (params->save_contours_())
  {
    vcl_string output_file;
    if (params->save_to_object_folder_()) 
      output_file = params->input_object_dir_() + "/";
    else {
      output_file = params->output_dir_() + "/";

      if (!vul_file::exists(output_file)) 
        vul_file::make_directory(output_file);
    }
    
    output_file = output_file + params->input_object_name_() + params->cem_output_extension_();

    // Save the contour fragment graph as a .cem file
    if (!dbdet_save_cem(output_file, EM, CFG)) {
      vcl_cout << "Problems in saving the contour file: " << output_file << vcl_endl;
      return 0;
    }
  }

  // 6) Evaluate the results
  
  //first read in the GT image
  vcl_string GT_img_filename = params->input_object_dir_() + "/" + params->input_object_name_() + params->GT_extension_();
  if (!vul_file::exists(GT_img_filename)) {
    vcl_cout << "Cannot find image: " << GT_img_filename << "\n";
    return 0;
  }

  vil_image_view<vxl_byte> GT_img  = vil_load(GT_img_filename.c_str());
  if (!image) {
    vcl_cout << "Cannot load image: " << GT_img_filename << "\n";
    return 0;
  }

  // call the edge linking evaluator
  dbdet_eval_edge_det_params eval_params(0, 5, 255, 2);

  //set the length thresholds
  for (double l=0; l<20; l++)
    eval_params.thresh.push_back(l);

  for (double l=25; l<100; l+=5)
    eval_params.thresh.push_back(l);

  //call the evaluator with the params above
  dbdet_eval_result res = dbdet_eval_edge_linking(eval_params, CFG, GT_img);

  // save the ROC information for this image
  vcl_string ROC_file;
  if (params->save_to_object_folder_()) 
    ROC_file = params->input_object_dir_() + "/";
  else {
    ROC_file = params->output_dir_() + "/";

    if (!vul_file::exists(ROC_file)) 
      vul_file::make_directory(ROC_file);
  }
  
  ROC_file = ROC_file + params->input_object_name_() + params->ROC_output_extension_();

  // Save the contour fragment graph as a .cem file
  res.save_ROC(ROC_file);

  return 0;
}

