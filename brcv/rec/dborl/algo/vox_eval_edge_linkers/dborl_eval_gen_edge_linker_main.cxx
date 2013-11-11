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


#include "dborl_eval_gen_edge_linker_params.h"
#include "dborl_eval_gen_edge_linker_params_sptr.h"

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>

#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/edge/dbdet_edgemap.h>

#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/sel/dbdet_sel_utils.h>

#include <dbdet/algo/dbdet_generic_linker.h>
#include <dbdet/algo/dbdet_postprocess_contours.h>

#include <dbdet/algo/dbdet_gen_edge_det.h>
#include <dbdet/algo/dbdet_third_order_edge_det.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/algo/dbdet_cem_file_io.h>

#include <dbdet/algo/dbdet_eval_edge_det.h>

int main(int argc, char *argv[]) 
{
  dborl_eval_gen_edge_linker_params_sptr params = new dborl_eval_gen_edge_linker_params("dborl_eval_gen_edge_linker");  // constructs with the default values;
  if (!params->parse_command_line_args(argc, argv))
    return 0;

  // always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
    vcl_cout << "problems in writing params file to: " << params->print_params_file() << vcl_endl;

  if (params->exit_with_no_processing() || params->print_params_only())
    return 0;

  // always call this method to actually parse the input parameter file whose name is extracted from the command line
  if (!params->parse_input_xml())
    return 0;

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

  // 2) detect edges
  int det_opt = params->edge_det_type_();        //detector option
  double sigma = params->edge_det_sigma_();      //sigma
  double threshold = params->edge_det_thresh_();  //threshold
  int N = params->edge_det_N_();                 //interpolation depth             

  bool adaptive_thresh = false;                  //adaptive threshold or not
  
  dbdet_edgemap_sptr EM; //the edge map 
  if (det_opt==1)
    EM = dbdet_detect_generic_edges(image, sigma, threshold, N);
  else if (det_opt==2)
    EM = dbdet_detect_third_order_edges(image, sigma, threshold, N, 0, 0, 0, adaptive_thresh, false, true);
  else
    return 1;

  // 3) save edgemap
  char out_edge_filename[300];
  //vcl_sprintf(out_edge_filename, "%s_%d_%f_%f.edg", out_prefix.c_str(), N, sigma, threshold);
  bool retval = dbdet_save_edg(vcl_string(out_edge_filename), EM);

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

  // 4) call the generic linker to link the edge map into contour fragments
  dbdet_curve_fragment_graph CFG; //the curve fragment graph

  if (params->link_edgels_())
  {
    //get the linking parameters
    bool req_app_con = params->req_app_con_();
    bool req_ori_con = false;
    bool req_smooth_con = false;
    bool req_low_ks = false;

    double app_thresh = params->app_thresh_();
    double len_thresh = params->len_thresh_();

    //required data structures for the edge linker
    vbl_array_2d<bool>  EULM;

    //construct a generic linker
    dbdet_generic_linker edge_linker(EM, CFG, EULM, threshold);

    //set various flags
    edge_linker.require_appearance_consistency(req_app_con);
    edge_linker.set_appearance_threshold(app_thresh);

    edge_linker.require_orientation_consistency(req_ori_con);//orientation consistency
    edge_linker.require_smooth_continuation(req_smooth_con);//smooth continuation
    edge_linker.require_low_curvatures(req_low_ks);//low curvatures

    //Extract the contours
    edge_linker.extract_image_contours_by_tracing();

    //prune contours by length
    prune_contours_by_length(CFG, len_thresh);

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

  if (!params->link_edgels_())
  {
    // call the edge detection evaluator only
    dbdet_eval_edge_det_params eval_params(0, 5, 255, 2);

    //set the edge thresholds
    double thresh_vec[] = {0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 10.0, 10.5, 11.0, 11.5, 12.0, 15.0, 18.0, 20.0, 25.0, 30.0, 40.0, 50.0};
    eval_params.thresh = vcl_vector<double>(thresh_vec, thresh_vec + sizeof(thresh_vec)/sizeof(thresh_vec[0]));

    //call the evaluator with the params above
    dbdet_eval_result res = dbdet_eval_edge_det(eval_params, EM, GT_img);

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
  }
  else {

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

  }

  return 0;
}

