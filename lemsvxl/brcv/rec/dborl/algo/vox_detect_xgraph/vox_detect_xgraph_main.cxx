// This is dborl/algo/vox_detect_xgraph/vox_detect_xgraph_main.cxx

//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date   Nov 2, 2008
// \brief An algorithm to detect objects represented by an extrinsic shock graph in images
// \verbatim
//    Modifications
// \endverbatim


#include "vox_detect_xgraph_params.h"
#include "vox_detect_xgraph_params_sptr.h"

#include <dborl/dborl_eval_det.h>
#include <dborl/algo/dborl_xio_image_description.h>

#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/algo/dbsks_vox_utils.h>
#include <dbsks/algo/dbsks_detect_xgraph_using_pyramid_edgemap.h>

#include <vsol/vsol_box_2d.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <cstdlib>

// =============================================================================
//: VOX executable to detect objects, represented by xshock_graph, in images
int main(int argc, char *argv[]) 
{

  vul_timer timer;
  timer.mark();

  //1. Parse the input xml file ////////////////////////////////////////////////
  std::cout
    << "\n---------------------------------------------------------------------"
    << "\nLoad parameter file"
    << "\n--------------------------------------------------------------------\n";

  // construct parameters with the default values;
  vox_detect_xgraph_params_sptr params = new vox_detect_xgraph_params("vox_detect_xgraph");

  // parse the command line arguments
  if (!params->parse_command_line_args(argc, argv))
  {
    return EXIT_FAILURE;
  }

  // always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
  {
    std::cout << "problems in writing params file to: " 
      << params->print_params_file() << std::endl;
  }

  // exit if there is nothing else to do
  if (params->exit_with_no_processing() || params->print_params_only())
  {
    return EXIT_SUCCESS;
  }

  // always call this method to actually parse the input parameter file whose 
  // name is extracted from the command line
  if (!params->parse_input_xml())
  {
    return EXIT_FAILURE;
  }


  //>> Write input.xml file to output folder
  // filename format: input+applelogos_another.xml
  params->print_input_xml(params->get_output_folder() + "/" + "input+" + 
    params->input_object_name_() + ".xml");

  
  
  //////////////////////////////////////////////////////////////////////////////
  //2. Detect objects in image
  
  //> Make some announcements
  std::cout 
    << "\n====================================================================="
    << "\nRun object detection on image: " << vul_file::strip_directory(params->get_image_file())
    << "\n  xgraph prototype: " << vul_file::strip_directory(params->get_xgraph_file()) 
    << "\n  geometry model:   " << vul_file::strip_directory(params->get_xgraph_geom_file())
    << "\n  ccm model:        " << vul_file::strip_directory(params->get_xgraph_ccm_file()) 
    << "\n====================================================================\n";
  

  //> Determine some global variables

  // Image to run detection on
  vil_image_resource_sptr image_resource = 0;

  // List of detections to return
  std::vector<dbsks_det_desc_xgraph_sptr > det_list;

  // Unique id for pair of (prototype xgraph, image)
  std::string det_group_id = params->get_xgraph_prototype_name() + "+" + params->input_object_name_();

  // Path to folder where we will save detection results in
  std::string work_folder = params->get_output_folder() + "/" + det_group_id;

  //> Switch object-detection procedures
  int choice = 1;
  

  //a. Use edge map pyramid and subpixel ccm - take input as a sub-pixel edgemap
  if (choice == 1)
  {  
    //> create the work directory if not yet done
    if (!vul_file::is_directory(work_folder))
    {
      vul_file::make_directory(work_folder);
    }

    //> Set up params
    dbsks_detect_xgraph_using_pyramid_edgemap m;

    // object name
    m.object_id = params->input_object_name_();
    
    // image file
    m.image_file = params->get_image_file();

    // edgemap files
    m.edgemap_folder           =  params->input_edgemap_folder_();
    m.edgemap_ext              = params->input_edgemap_extension_();
    m.edgeorient_ext           = params->input_edgeorient_extension_();
    m.edgemap_log2_scale_ratio = params->input_edgemap_log2_scale_ratio_();

    // xgraph protype + model
    m.xgraph_file              = params->get_xgraph_file();
    m.xgraph_geom_file         = params->get_xgraph_geom_file();
    m.xgraph_geom_param_file   = params->get_xgraph_geom_param_file();
    m.xgraph_ccm_file          = params->get_xgraph_ccm_file();
    m.xgraph_ccm_param_file    = params->get_xgraph_ccm_param_file();
    m.cfrag_list_to_ignore     = params->get_cfrag_list_to_ignore();
    
    // xgraph scales
    m.prototype_xgraph_min_size                   = params->prototype_xgraph_min_size_();
    m.prototype_xgraph_log2_increment_step        = params->prototype_xgraph_log2_increment_step_();
    m.prototype_xgraph_max_size                   = params->prototype_xgraph_max_size_();
    m.prototype_xgraph_ratio_max_size_to_min_size = params->prototype_xgraph_ratio_max_size_to_min_size_();
    
    // detection window
    m.det_window_width  = params->detection_window_width_();
    m.det_window_height = params->detection_window_height_();

    // work folder
    m.work_folder = work_folder;

    // filtering output
    m.min_accepted_confidence         = params->min_accepted_confidence_();
    m.run_nms_based_on_overlap        = params->run_nms_based_on_overlap_();
    m.min_overlap_ratio_for_rejection = params->min_overlap_ratio_for_rejection_();

    //> Run detection
    bool success = m.execute();

    if (!success)
    {
      std::cout << "\nERROR: Xgraph detection failed. No results created." << std::endl;
      return EXIT_FAILURE;
    }

    //> Collect detection resulst
    det_list = m.output_det_list;
    image_resource = vil_load_image_resource(params->get_image_file().c_str());
  }
  else
  {
    std::cout << "\nERROR: Unknown choice." << std::endl;
    return EXIT_FAILURE;
  }

  //> Sort the detection results based in the order of decreasing confidence
  std::sort(det_list.begin(), det_list.end(), dbsks_decreasing_confidence);
  std::cout << "\n  Final number of detections= " << det_list.size() << std::endl;

  //////////////////////////////////////////////////////////////////////////////
  // 3. Save detection results

  std::cout
    << "\n---------------------------------------------------------------------"
    << "\nSave detection results"
    << "\n--------------------------------------------------------------------\n";

  //> Save summary in VOC2008 format
  std::string det_result_file = params->get_output_folder() + "/" + 
    "VOC_det_result+" + params->get_xgraph_prototype_name() +
    "+" + params->input_object_name_() + ".txt";

  std::cout << "\n>> Save summary in VOC2008 format: " << det_result_file << "...";
  {
    // \todo turn this into a function
    std::ofstream ofs(det_result_file.c_str(), std::ios::out);
    for (unsigned i =0; i < det_list.size(); ++i)
    {
      vsol_box_2d_sptr bbox = det_list[i]->bbox();
      ofs << params->input_object_name_() << " "
        << det_list[i]->confidence() << " "
        << bbox->get_min_x() << " "
        << bbox->get_min_y() << " "
        << bbox->get_max_x() << " "
        << bbox->get_max_y() << "\n";
    }
    ofs.close();
  }
  std::cout << "done.\n";

  //> Save each detection: xgraph + screenshot
  std::cout << "\n>> Save each detected xgraph to output folder: " 
    << params->get_output_folder() << "...";

  // Original image
  vil_image_view<vxl_byte > source_image = *vil_convert_cast(vxl_byte(), image_resource->get_view());

  // Write all detections to a folder within the output folder
  dbsks_save_detections_to_folder(det_list, params->input_object_name_(), 
    params->input_model_category_(), 
    det_group_id, 
    source_image,
    params->get_output_folder(),
    det_group_id);
  std::cout << "done.\n";

  //////////////////////////////////////////////////////////////////////////////
  // 4. Evaluate detection results (compare against groundtruth)

  std::cout
    << "\n---------------------------------------------------------------------"
    << "\nEvaluate detection results"
    << "\n--------------------------------------------------------------------\n";
  
  //> Load groundtruth file
  std::string image_desc_file = params->input_object_dir_() + "/" + params->input_object_name_() + ".xml";

  std::cout << "\n>> Load groundtruth file: " << image_desc_file << "...";

  borld_image_description_sptr image_desc;
  if (!x_read(image_desc_file, image_desc))
  {
    std::cout << "Failed.\n";
    return EXIT_FAILURE;
  }
  else
  {
    std::cout << "Done.\n";
  }

  //> Evaluate the detection results against objects in the image description
  std::cout << "\n>> Compare each detection against ground truth data ...";
  double min_required_overlap = 0.5; // PASCAL criteria
  std::vector<double > confidence_vec;
  std::vector<int > FP_vec;
  std::vector<int > TP_vec;
  int num_pos = -1;

  // Clone the detection list to its parent class
  std::vector<dborl_det_desc_sptr > borl_det_list;
  borl_det_list.reserve(det_list.size());
  for (unsigned i =0; i < det_list.size(); ++i)
  {
    borl_det_list.push_back(det_list[i].ptr());
  }
  dborl_VOC2008_eval_det(image_desc, params->input_model_category_(), min_required_overlap, borl_det_list,
    confidence_vec, FP_vec, TP_vec, num_pos);
  std::cout << "\nFinished comparing against ground truth.\n";

  //> Print out the evaluation results
  std::string eval_file = params->get_output_folder() + "/" + 
    "eval_det+" + params->get_xgraph_prototype_name() + "+" + 
    params->input_object_name_() + ".txt";

  std::cout << "\n>> Print out evaluation results to file: " << eval_file << std::endl;
  dborl_VOC2008_print_eval_results(eval_file, num_pos, params->input_object_name_(),
    params->input_model_category_(), confidence_vec, TP_vec, FP_vec);

  // Display total time spent
  std::cout << "\n\n>> Total time spent for the task: " 
    << timer.all() / 1000 << " seconds.\n" << std::endl;  
  
  return EXIT_SUCCESS;
};

