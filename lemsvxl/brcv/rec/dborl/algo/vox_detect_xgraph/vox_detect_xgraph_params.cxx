// This is rec/dborl/algo/vox_detect_xgraph/vox_detect_xgraph_params.cxx

//:
// \file


#include "vox_detect_xgraph_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <vul/vul_file.h>
//#include <dborl/algo/dborl_utilities.h>

// -----------------------------------------------------------------------------
//: Constructor
vox_detect_xgraph_params::
vox_detect_xgraph_params(vcl_string algo_name) : 
  dborl_algo_params(algo_name) 
{
  // Image //////////////////////////////////////////////////

  //: Get input image from object folder?
  this->input_image_from_object_folder_.set_values(this->param_list_, "io",
    "input_image_from_object_folder", "[DATA] read original image from object folder?", true, false);

  //: folder containing the images (if not from object folder)
  this->input_image_folder_.set_values(this->param_list_, "io", "input_image_folder",
    "[DATA] input folder to read image file", 
    "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals",
    "v:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals");

  // extension of the input image
  this->input_image_extension_.set_values(this->param_list_, "io", "input_image_extention", 
    "[DATA] input image extension", 
    ".jpg", 
    ".jpg");


  // Edgemap and edge orientation /////////////////////////////////////////////

  // Where to get edgemap
  this->input_edgemap_from_object_folder_.set_values(this->param_list_, "io",
    "input_edgemap_from_object_folder", "[DATA] read Pb edgemap from object folder?", false, false);

  // Folder of input edgemap (if not in object folder)
  this->input_edgemap_folder_.set_values(this->param_list_, "io", "input_edgemap_folder",
    "[DATA] input folder to read edgemap file", 
    //"/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_edgemaps-clean_using_kovesi-I_15-len_10",
    "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4",
    "v:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4");

  // extension of the input edgemap
  this->input_edgemap_extension_.set_values(this->param_list_, "io", "input_edgemap_extention", 
    "[DATA] input edgemap extension", 
    "_pb_edges.png",  //"_edges.tif"
    "_pb_edges.png" //"_edges.tif"
    );

  //// Folder of input edgemap orientation (if not in object folder)
  //this->input_edgeorient_folder_.set_values(this->param_list_, "io", "input_edgeorient_folder",
  //  "[Depreciated] [DATA] input folder to read edge orientation file", 
  //  //"/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_edgeorients-txt-clean_using_kovesi-I_15-len_10",
  //  "dummy", 
  //  "dummy");
  
  // extension of the input edge orientation
  this->input_edgeorient_extension_.set_values(this->param_list_, "io", "input_edgeorient_extention", 
    "[DATA] input edge orientation extension", 
    "_pb_orient.txt", //"_orient.txt", 
    "_pb_orient.txt"); //"_orient.txt");

  //// Folder of input edgemap orientation (if not in object folder)
  //this->input_cemv_folder_.set_values(this->param_list_, "io", "input_cemv_folder",
  //  "[Depreciated] [DATA] input folder to read linked-edge file", 
  //  //"/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_cemv-using_kovesi-I_15-len_10",
  //  "dummy",
  //  "dummy");

    // extension of the input edge orientation
  this->input_cemv_extension_.set_values(this->param_list_, "io", "input_cemv_extension", 
    "[DATA] input linked-edge file extension", 
    "_pb_linked.cem", //".cemv", 
    "_pb_linked.cem"); // ".cemv");


  //: Relative scale of edgemap used for detection.
  // value is in log2 of the scale ratio between of the selected edgemap and the selected image in the pyramid
  this->input_edgemap_log2_scale_ratio_.set_values(this->param_list_, "io", "input_edgemap_log2_scale_ratio", 
    "[EDGE] Log2 of relative scale of edgemap used for detection", 
    0.5f, // use edgemap at the same level as the image
    0.5f);


  ////: Base size of prototype xgraph - all detections will be run on this size
  //// \todo This will be detetermined by the ccm xgraph model
  //this->prototype_xgraph_base_size_.set_values(this->param_list_, "io", "prototype_xgraph_base_size", 
  //  "[MODEL-SIZE] prototype_xgraph_base_size (sqrt of area)", 
  //  64.0f, // use edgemap at the same level as the image
  //  64.0f);

  //: Min graph size
  this->prototype_xgraph_min_size_.set_values(this->param_list_, "io", "prototype_xgraph_min_size", 
    "[MODEL-SIZE] prototype_xgraph_min_size (sqrt of area)", 
    50.0f, 
    50.0f);

  //: Max graph size
  this->prototype_xgraph_max_size_.set_values(this->param_list_, "io", "prototype_xgraph_max_size", 
    "[MODEL-SIZE] prototype_xgraph_max_size (sqrt of area)", 
    1000.0f,
    1000.0f);

  //: Increment step of graph size - computed as log2 of scale ratio
  this->prototype_xgraph_log2_increment_step_.set_values(this->param_list_, "io", "prototype_xgraph_log2_increment_step", 
    "[MODEL-SIZE] prototype_xgraph_log2_increment_step, e.g. step (50->100) = 1", 
    0.5f,
    0.5f);
  
  //: Max ratio between largest and smallest model size - in log2
  this->prototype_xgraph_ratio_max_size_to_min_size_.set_values(this->param_list_, "io", "prototype_xgraph_ratio_max_size_to_min_size", 
    "[MODEL-SIZE] Max ratio between max-size and min-size", 
    9.0f,
    9.0f);

  //: xgraph Contour-Chamfer-Matching model param filename (.xml)
  this->input_xgraph_ccm_param_filename_.set_values(this->param_list_, "io", "input_xgraph_ccm_param_filename",
          "[MODEL-PARAM] Input xgraph Contour-Chamfer-Matching model parameter filename (full path)",
          "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/ccm_params.xml",
          "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/ccm_params.xml");

  //: xgraph Contour-Chamfer-Matching model param filename (.xml)
  this->input_xgraph_geom_param_filename_.set_values(this->param_list_, "io", "input_xgraph_geom_param_filename",
          "[MODEL-PARAM] Input xgraph geom model parameter filename (full path)",
          "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/geom_params.xml",
          "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/geom_params.xml");

  // Category-spectific info ////////////////////////////////////////////////////////

  // a hack to change default info for debugging purpose
  vcl_string category = "swans";

  if (category == "giraffes")
  {
    //: Object categery of input shock graph
    this->input_model_category_.set_values(this->param_list_, "io", "input_model_category", 
      "input model category", 
      "giraffes", 
      "giraffes");

    // Name of input object
    this->input_object_name_.set_values(this->param_list_, "io", "input_object_name",
      "input object name", 
      "dummy", 
      "giraffes_green",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    // passes the folder of the input object
    this->input_object_dir_.set_values(this->param_list_, "io", "input_object_dir",
      "input object folder", 
      "/vision/projects/kimia/categorization/ethz_shape/vox-upload/giraffes_green", 
      "v:/projects/kimia/categorization/ethz_shape/vox-upload/giraffes_green",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR);

    //: Folder containing the object shock graph
    this->input_xgraph_folder_.set_values(this->param_list_, "io", "input_xgraph_folder",
      "[XGRAPH] input folder to read xgraph file", 
      "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/giraffes-xgraph",
      "v:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/giraffes-xgraph");

    // XML filename of object
    this->input_xgraph_prototype_filename_.set_values(this->param_list_, "io", "input_xgraph_name", 
      "[XGRAPH] input xgraph name", 
      "giraffes_green.xgraph.0.xml", 
      "giraffes_green.xgraph.0.xml");

    //: xgraph geometric model filename (.xml)
    this->input_xgraph_geom_filename_.set_values(this->param_list_, "io", "input_xgraph_geom_filename",
      "[GEOM] Input xgraph geometric model filename",
      "giraffes-xgraph_geom_model-v2-2009sep02.xml",
      "giraffes-xgraph_geom_model-v2-2009sep02.xml");


    //: xgraph Contour-Chamfer-Matching model filename (.xml)
    this->input_xgraph_ccm_filename_.set_values(this->param_list_, "io", "input_xgraph_ccm_filename",
      "[CCM] Input xgraph Contour-Chamfer-Matching model filename",
      "giraffes-xgraph_ccm_model-v2-2009aug31-chamfer_4.xml",
      "giraffes-xgraph_ccm_model-v2-2009aug31-chamfer_4.xml");

    //: List of contour fragments whose cost will be ignored
    this->input_cfrag_list_to_ignore_.set_values(this->param_list_, "io", "input_cfrag_list_to_ignore", 
      "[CCM] Input list of bnd contour fragments to ignore cost (separated by space commas)", 
      "0-L,0-R", 
      "0-L,0-R");    
  }
  else if (category == "bottles")
  {
      //: Object categery of input shock graph
    this->input_model_category_.set_values(this->param_list_, "io", "input_model_category", 
      "input model category", 
      "bottles", 
      "bottles");

    // Name of input object
    this->input_object_name_.set_values(this->param_list_, "io", "input_object_name",
      "input object name", 
      "dummy", 
      "bottles_acaw",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    // passes the folder of the input object
    this->input_object_dir_.set_values(this->param_list_, "io", "input_object_dir",
      "input object folder", 
      "/vision/projects/kimia/categorization/ethz_shape/vox-upload/bottles_acaw", 
      "v:/projects/kimia/categorization/ethz_shape/vox-upload/bottles_acaw",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR);


    //: Folder containing the object shock graph
    this->input_xgraph_folder_.set_values(this->param_list_, "io", "input_xgraph_folder",
      "[XGRAPH] input folder to read xgraph file", 
      "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/bottles",
      "v:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/bottles");

    // XML filename of object
    this->input_xgraph_prototype_filename_.set_values(this->param_list_, "io", "input_xgraph_name", 
      "[XGRAPH] input xgraph name", 
      "bottles_prototype.0.xml", 
      "bottles_prototype.0.xml");


    //: xgraph geometric model filename (.xml)
    this->input_xgraph_geom_filename_.set_values(this->param_list_, "io", "input_xgraph_geom_filename",
      "[GEOM] Input xgraph geometric model filename",
      "bottles-xgraph_geom_model-v2-20jan25.xml",
      "bottles-xgraph_geom_model-v2-20jan25.xml");


    //: xgraph Contour-Chamfer-Matching model filename (.xml)
    this->input_xgraph_ccm_filename_.set_values(this->param_list_, "io", "input_xgraph_ccm_filename",
      "[CCM] Input xgraph Contour-Chamfer-Matching model filename",
      "bottles-xgraph_ccm_model-v2-2010jan22-chamfer_4.xml",
      "bottles-xgraph_ccm_model-v2-2010jan22-chamfer_4.xml");

    //: List of contour fragments whose cost will be ignored
    this->input_cfrag_list_to_ignore_.set_values(this->param_list_, "io", "input_cfrag_list_to_ignore", 
      "[CCM] Input list of bnd contour fragments to ignore cost (separated by space commas)", 
      "14-L,14-R,19-L,19-R", 
      "14-L,14-R,19-L,19-R");
  }

  // swans
  else if (category == "swans")
  {
      //: Object categery of input shock graph
    this->input_model_category_.set_values(this->param_list_, "io", "input_model_category", 
      "input model category", 
      "swans", 
      "swans");

    // Name of input object
    this->input_object_name_.set_values(this->param_list_, "io", "input_object_name",
      "input object name", 
      "dummy", 
      "swans_aal",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    // passes the folder of the input object
    this->input_object_dir_.set_values(this->param_list_, "io", "input_object_dir",
      "input object folder", 
      "/vision/projects/kimia/categorization/ethz_shape/vox-upload/swans_aal", 
      "v:/projects/kimia/categorization/ethz_shape/vox-upload/swans_aal",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR);


    //: Folder containing the object shock graph
    this->input_xgraph_folder_.set_values(this->param_list_, "io", "input_xgraph_folder",
      "[MODEL] input folder to read xgraph file", 
      "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/swans",
      "v:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/swans");

    // XML filename of object
    this->input_xgraph_prototype_filename_.set_values(this->param_list_, "io", "input_xgraph_name", 
      "[MODEL] input xgraph name", 
      "swans_prototype_0.xml", 
      "swans_prototype_0.xml");


    //: xgraph geometric model filename (.xml)
    this->input_xgraph_geom_filename_.set_values(this->param_list_, "io", "input_xgraph_geom_filename",
      "[MODEL] Input xgraph geometric model filename",
      "swans_prototype_0-xshock_geom_model-2010feb02.xml",
      "swans_prototype_0-xshock_geom_model-2010feb02.xml");

    //: xgraph Contour-Chamfer-Matching model filename (.xml)
    this->input_xgraph_ccm_filename_.set_values(this->param_list_, "io", "input_xgraph_ccm_filename",
      "[MODEL] Input xgraph Contour-Chamfer-Matching model filename",
      "swans_prototype_0-xshock_ccm_model-2010feb02.xml",
      "swans_prototype_0-xshock_ccm_model-2010feb02.xml");

    //: List of contour fragments whose cost will be ignored
    this->input_cfrag_list_to_ignore_.set_values(this->param_list_, "io", "input_cfrag_list_to_ignore", 
      "[MODEL] Input list of bnd contour fragments to ignore cost (separated by space commas)", 
      "0-L,0-R", 
      "0-L,0-R");
  }


  // mugs
  else if (category == "mugs")
  {
      //: Object categery of input shock graph
    this->input_model_category_.set_values(this->param_list_, "io", "input_model_category", 
      "[INPUT] input model category", 
      "mugs", 
      "mugs");

    // Name of input object
    this->input_object_name_.set_values(this->param_list_, "io", "input_object_name",
      "[INPUT] input object name", 
      "mugs_tdnkitchen", 
      "mugs_tdnkitchen",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    // passes the folder of the input object
    this->input_object_dir_.set_values(this->param_list_, "io", "input_object_dir",
      "[INPUT] input object folder", 
      "/vision/projects/kimia/categorization/ethz_shape/vox-upload/mugs_tdnkitchen", 
      "v:/projects/kimia/categorization/ethz_shape/vox-upload/mugs_tdnkitchen",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR);


    //: Folder containing the object shock graph
    this->input_xgraph_folder_.set_values(this->param_list_, "io", "input_xgraph_folder",
      "[XGRAPH] input folder to read xgraph file", 
      "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/mugs-xgraph",
      "v:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/mugs-xgraph");

    // XML filename of object
    this->input_xgraph_prototype_filename_.set_values(this->param_list_, "io", "input_xgraph_name", 
      "[XGRAPH] input xgraph name", 
      "mugs_blue.xgraph.0.xml", 
      "mugs_blue.xgraph.0.xml");

    // option 1: degree-3 root node
    //: xgraph geometric model filename (.xml)
    this->input_xgraph_geom_filename_.set_values(this->param_list_, "io", "input_xgraph_geom_filename",
      "[GEOM] Input xgraph geometric model filename",
      "xgraph_geom_model-mugs-v2.xml",
      "xgraph_geom_model-mugs-v2.xml");

    //: xgraph Contour-Chamfer-Matching model filename (.xml)
    this->input_xgraph_ccm_filename_.set_values(this->param_list_, "io", "input_xgraph_ccm_filename",
      "[CCM] Input xgraph Contour-Chamfer-Matching model filename",
      "mugs-xgraph_ccm_model-v2-2009aug26-chamfer_4.xml",
      "mugs-xgraph_ccm_model-v2-2009aug26-chamfer_4.xml");

    //: List of contour fragments whose cost will be ignored
    this->input_cfrag_list_to_ignore_.set_values(this->param_list_, "io", "input_cfrag_list_to_ignore", 
      "[CCM] Input list of bnd contour fragments to ignore cost (separated by commas)", 
      "0-L,0-R", 
      "0-L,0-R");
  }

  // applelogos
  else if (category == "applelogos")
  {
    //: Object categery of input shock graph
    this->input_model_category_.set_values(this->param_list_, "io", "input_model_category", 
      "[INPUT] input model category", 
      "applelogos", 
      "applelogos");

    // Name of input object
    this->input_object_name_.set_values(this->param_list_, "io", "input_object_name",
      "[INPUT] input object name", 
      "dummy", 
      "applelogos_another",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    // passes the folder of the input object
    this->input_object_dir_.set_values(this->param_list_, "io", "input_object_dir",
      "[INPUT] input object folder", 
      "/vision/projects/kimia/categorization/ethz_shape/vox-upload/applelogos_another", 
      "v:/projects/kimia/categorization/ethz_shape/vox-upload/applelogos_another",
      0, // for 0th input object
      dborl_parameter_system_info::INPUT_OBJECT_DIR);


    //: Folder containing the object shock graph
    this->input_xgraph_folder_.set_values(this->param_list_, "io", "input_xgraph_folder",
      "[XGRAPH] input folder to read xgraph file", 
      "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/applelogos-xgraph",
      "v:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/applelogos-xgraph");

    // XML filename of object
    this->input_xgraph_prototype_filename_.set_values(this->param_list_, "io", "input_xgraph_name", 
      "[XGRAPH] input xgraph name", 
      "applelogos_another.xgraph.0.prototype1.xml", 
      "applelogos_another.xgraph.0.prototype1.xml");

    // option 1: degree-1 root node
    //: xgraph geometric model filename (.xml)
    this->input_xgraph_geom_filename_.set_values(this->param_list_, "io", "input_xgraph_geom_filename",
      "[GEOM] Input xgraph geometric model filename",
      "xgraph_geom_model-applelogos_prototype1.xml",
      "xgraph_geom_model-applelogos_prototype1.xml");

    //: xgraph Contour-Chamfer-Matching model filename (.xml)
    this->input_xgraph_ccm_filename_.set_values(this->param_list_, "io", "input_xgraph_ccm_filename",
      "[CCM] Input xgraph Contour-Chamfer-Matching model filename",
      "applelogos_prototype1-ccm_cost-2009jul14-model-v2.xml",
      "applelogos_prototype1-ccm_cost-2009jul14-model-v2.xml");
    

    //: List of contour fragments whose cost will be ignored
    this->input_cfrag_list_to_ignore_.set_values(this->param_list_, "io", "input_cfrag_list_to_ignore", 
      "[CCM] Input list of bnd contour fragments to ignore cost (separated by commas)", 
      "9-L,9-R,28-L,28-R,34-L,34-R", 
      "9-L,9-R,28-L,28-R,34-L,34-R");  
  }
  else
  {
    vcl_cout << "\nERROR!!!! Unknown category. Many parameters were not set properly.\n";
  }



  //: Parameter lambda of Oriented Chamfer Matching
  this->input_wcm_weight_unmatched_.set_values(this->param_list_, "io", "input_wcm_unmatched", 
    "[WCM] Weight of cost for unmatched edges in WCM cost computation", 
    0.0f, 
    0.0f);

  
  //////////////////////////////////////////////////////////////////////////////
  // Output 

  // Save result to the object folder?
  this->save_to_object_folder_.set_values(this->param_list_, "io", "save_to_object_folder", 
    "[OUTPUT] save result to object folder?", false, false);
  
  // if written to this folder as opposed to object folder 
  this->output_object_folder_.set_values(this->param_list_, "io", "output_object_folder", 
    "[OUTPUT] output folder to write results", 
    "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments",
    "D:/vision/projects/symseg/xshock/vox-output");

  // tag for this algorithm
  this->algo_abbreviation_ = "detect_xgraph";

  //////////////////////////////////////////////////////////////////////////////
  //>> External constraints

  //: Width of detection window
  this->detection_window_width_.set_values(this->param_list_, "io", 
    "detection_window_width", 
    "[IMPL] Width of detection window", int(256), int(256)); //int(512), int(512));

  //: Height of detection window
  this->detection_window_height_.set_values(this->param_list_, "io", 
    "detection_window_height", 
    "[IMPL] Height of detection window", int(256), int(256)); //int(512), int(512));


  //////////////////////////////////////////////////////////////////////////////
  //> Post-process
  //: (Absolute) Minimum confidence value to be accepted as a detection
  this->min_accepted_confidence_.set_values(this->param_list_, "io", 
    "min_accepted_confidence", 
    "[POSTPROC] Minimum accepted confidence level", 20.0f, 20.0f);

  //: Run non-max suppression based on overlapping?
  this->run_nms_based_on_overlap_.set_values(this->param_list_, "io",
    "run_nms_based_on_overlap",
    "[POSTPROC] Run NMS based on overlap?", true, true);

  //: Minimum overlap between two detection before one is rejected
  this->min_overlap_ratio_for_rejection_.set_values(this->param_list_, "io", 
    "min_overlap_ratio_for_rejection", 
    "[POSTPROC] Min overlap ratio to be considered overlap during NMS", 0.5f, 0.5f);
}



//// -----------------------------------------------------------------------------
////: List of scales
//bool vox_detect_xgraph_params::
//parse_input_model_scales(vcl_vector<double >& model_scales) const
//{
//  vcl_string scales_string = this->input_model_scales_();
//  
//  // replace "," by space so that we can parse using vector's function
//  for (unsigned i =0; i < scales_string.length(); ++i)
//  {
//    if (scales_string[i] == ',')
//    {
//      scales_string[i] =' ';
//    }
//  }
//  
//  // parse the scale string using stringstream
//  vcl_stringstream ss;
//  ss << scales_string;
//
//  // keep parsing till the end of the string
//  double scale;
//  while (ss >> scale)
//  {
//    model_scales.push_back(scale);
//  }
//  return true;
//}



//------------------------------------------------------------------------------
//: List of boundary contour fragment to ignore
bool vox_detect_xgraph_params::
parse_input_cfrag_list_to_ignore(vcl_vector<vcl_string >& cfrag_list_to_ignore) const
{
  // clean-up first
  cfrag_list_to_ignore.clear();


  // parse the corresponding parameter
  vcl_string buffer = this->input_cfrag_list_to_ignore_();
  
  // replace "," by space so that we can use standard parser
  for (unsigned i =0; i < buffer.length(); ++i)
  {
    if (buffer[i] == ',')
    {
      buffer[i] =' ';
    }
  }
  
  // parse the string using stringstream
  vcl_stringstream ss;
  ss << buffer;

  vcl_string cfrag_desc;
  while (ss >> cfrag_desc)
  {
    cfrag_list_to_ignore.push_back(cfrag_desc);
  }
  return true;
}







//------------------------------------------------------------------------------
//:
vcl_string vox_detect_xgraph_params::
get_image_file() const
{
  vcl_string image_folder;
  if (this->input_image_from_object_folder_())
  {
    image_folder = this->input_object_dir_();
  }
  else
  {
    image_folder = this->input_image_folder_();
  }
  vcl_string image_file = image_folder + "/" + this->input_object_name_() + 
    this->input_image_extension_();
  return image_file;
}

  
//------------------------------------------------------------------------------  
//:
vcl_string vox_detect_xgraph_params::
get_xgraph_file() const
{
  vcl_string xgraph_file = this->input_xgraph_folder_()+ "/" +this->input_xgraph_prototype_filename_();
  return xgraph_file;
}

//------------------------------------------------------------------------------
//:
vcl_string vox_detect_xgraph_params::
get_edgemap_file() const
{
  vcl_string edgemap_folder;
  if (this->input_edgemap_from_object_folder_()) 
  {
    edgemap_folder = this->input_object_dir_();
  }
  else
  {
    edgemap_folder = this->input_edgemap_folder_();
  }
  vcl_string edgemap_file = edgemap_folder + "/" + this->input_object_name_() + 
    this->input_edgemap_extension_();

  return edgemap_file;
}


////------------------------------------------------------------------------------
////:
//vcl_string vox_detect_xgraph_params::
//get_edgeorient_file() const
//{
//  // edge orientation
//  vcl_string edgeorient_folder;
//  if (this->input_edgemap_from_object_folder_()) 
//  {
//    edgeorient_folder = this->input_object_dir_();
//  }
//  else
//  {
//    edgeorient_folder = this->input_edgeorient_folder_();
//  }
//  vcl_string edgeorient_file = edgeorient_folder + "/" + this->input_object_name_() + 
//    this->input_edgeorient_extension_();
//  return edgeorient_file;
//}


//------------------------------------------------------------------------------
//: xgraph geometric model
vcl_string vox_detect_xgraph_params::
get_xgraph_geom_file() const
{
  vcl_string xgraph_geom_file = this->input_xgraph_folder_()+ "/" +this->input_xgraph_geom_filename_();
  return xgraph_geom_file;
}

//: xgraph geometric model
vcl_string vox_detect_xgraph_params::
get_xgraph_geom_param_file() const
{
  vcl_string xgraph_geom_param_file = this->input_xgraph_geom_param_filename_();
  return xgraph_geom_param_file;
}

//------------------------------------------------------------------------------
//:
vcl_vector<double > vox_detect_xgraph_params::
get_xgraph_scales() const
{
  vcl_vector<double > xgraph_scales;
  ////
  //if (this->parse_input_model_scales(xgraph_scales))
  //{
  //  return xgraph_scales;
  //}
  //else
  //{
  //  return vcl_vector<double >();
  //}
  return xgraph_scales;
}


//------------------------------------------------------------------------------
//:
vcl_string vox_detect_xgraph_params::
get_xgraph_ccm_file() const
{
  vcl_string xgraph_ccm_file = this->input_xgraph_folder_()+ "/" +this->input_xgraph_ccm_filename_();
  return xgraph_ccm_file;
}

//:
vcl_string vox_detect_xgraph_params::
get_xgraph_ccm_param_file() const
{
  vcl_string xgraph_ccm_param_file = this->input_xgraph_ccm_param_filename_();
  return xgraph_ccm_param_file;
}


////------------------------------------------------------------------------------
////:
//vcl_string vox_detect_xgraph_params::
//get_cemv_file() const
//{
//  vcl_string cemv_folder;
//  if (this->input_edgemap_from_object_folder_()) 
//  {
//    cemv_folder = this->input_object_dir_();
//  }
//  else
//  {
//    cemv_folder = this->input_cemv_folder_();
//  }
//  vcl_string cemv_file = cemv_folder + "/" + this->input_object_name_() + 
//    this->input_cemv_extension_();
//  return cemv_file;
//}


//------------------------------------------------------------------------------
//: list of boundary fragments to ignore
vcl_vector<vcl_string > vox_detect_xgraph_params::
get_cfrag_list_to_ignore() const
{
  vcl_vector<vcl_string > cfrag_list_to_ignore;
  if (this->parse_input_cfrag_list_to_ignore(cfrag_list_to_ignore))
  {
    return cfrag_list_to_ignore;
  }
  else
  {
    return vcl_vector<vcl_string >();
  }
}


// location of all output files
vcl_string vox_detect_xgraph_params::
get_output_folder() const
{
  if (this->save_to_object_folder_())
  {
    return this->input_object_dir_();
  }
  else
  {
    return this->output_object_folder_();
  }
}



//: Name of the prototype xgraph
vcl_string vox_detect_xgraph_params::
get_xgraph_prototype_name() const
{
  return vul_file::strip_extension(this->input_xgraph_prototype_filename_());
}
