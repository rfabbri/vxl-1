//:
// \file
// \author Amir Tamrakar
// \date 09/08/08
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


#include "dborl_eval_VD_params.h"
#include "dborl_eval_VD_params_sptr.h"

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>

//required because sdet still uses vil1
#include <vil1/vil1_vil.h>
#include <vil1/vil1_memory_image_of.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_curve_2d_sptr.h>

#include <vtol/vtol_edge_2d.h>

#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>

#include <sdet/sdet_detector.h>
#include <sdet/sdet_detector_params.h>

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
  dborl_eval_VD_params_sptr params = new dborl_eval_VD_params("dborl_eval_VD");  // constructs with the default values;
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

  //convert to grayscale
  vil_image_view<vxl_byte> grey_view;
  if(image.nplanes() == 3){
    vil_convert_planes_to_grey(image, grey_view );
  }
  else if (image.nplanes() == 1){
    grey_view = image;
  }

  //make a vil1 image
  vil1_memory_image_of< unsigned char > img = vil1_from_vil_image_view( grey_view );

  // 2) detect edges
  sdet_detector_params dp;
  dp.smooth = params->edge_det_sigma_();                          //sigma
  dp.noise_multiplier = params->edge_det_thresh_();               //noise threshold
  dp.automatic_threshold = params->edge_det_auto_thresh_();       // automatic threshold
  dp.aggressive_junction_closure = params->edge_det_agrresive_(); // aggressive closure
  dp.minLength  = params->edge_det_len_thresh_();                 //minimum length threshold
  dp.borderp = false;                                             //include borders

  //initialize the detector
  sdet_detector detector(dp);
  detector.SetImage(img);
  //process edges
  detector.DoContour();
  vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

  //create an edgemap from the detector output
  dbdet_edgemap_sptr EM = new dbdet_edgemap(img.width(), img.height());
  vcl_vector<dbdet_edgel_chain*> chains; //to store the edgel chains
  for (unsigned i=0; i<edges->size(); i++)
  {
    vtol_edge_2d_sptr ve = (*edges)[i];
    vdgl_digital_curve_sptr dc = ve->cast_to_edge_2d()->curve()->cast_to_vdgl_digital_curve();

    unsigned len = dc->get_interpolator()->get_edgel_chain()->size();

    //create a new chain (for each vtol edge)
    dbdet_edgel_chain* chain = new dbdet_edgel_chain();
    for (unsigned j=0; j<len; j++){
      vdgl_edgel edgel = dc->get_interpolator()->get_edgel_chain()->edgel(j);
      dbdet_edgel* e = new dbdet_edgel(edgel.get_pt(), edgel.get_theta()*vnl_math::pi/180.0 - vnl_math::pi_over_2, edgel.get_grad()); 
      EM->insert(e);
      chain->push_back(e);
    }
    //add this chain to the list of edgel chains
    chains.push_back(chain);
  }

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

  // 4) Create a CFG from the vtol edges
  dbdet_curve_fragment_graph CFG; //the curve fragment graph
  CFG.resize(EM->edgels.size());
  for (unsigned i=0; i<chains.size(); i++)
    CFG.insert_fragment(chains[i]);

  // 4) save the linked contour files
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

  // 5) Evaluate the results
  
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

  if (params->eval_edges_only_())
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

