// This is dborl/algo/vox_detect_xgraph2/vox_detect_xgraph2_main.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 19, 2010
// \brief An algorithm to detect objects represented by an extrinsic shock graph in images
// \verbatim
//    Modifications
// \endverbatim


#include "vox_detect_xgraph2_params.h"
#include "vox_detect_xgraph2_params_sptr.h"

#include <dborl/dborl_eval_det.h>
#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/algo/dbsks_vox_utils.h>
#include <dborl/algo/dborl_xio_image_description.h>

#include "vox_detect_xgraph_using_pyramid_edgemap.h"

#include <vsol/vsol_box_2d.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vcl_cstdlib.h>

// =============================================================================
//: VOX executable to detect objects, represented by xshock_graph, in images
int main(int argc, char *argv[]) 
{

  vul_timer timer;
  timer.mark();

  //1. Parse the input xml file ////////////////////////////////////////////////
  vcl_cout
    << "\n---------------------------------------------------------------------"
    << "\nLoad parameter file"
    << "\n--------------------------------------------------------------------\n";

  // construct parameters with the default values;
  vox_detect_xgraph2_params_sptr params = new vox_detect_xgraph2_params("vox_detect_xgraph2");

  // parse the command line arguments
  if (!params->parse_command_line_args(argc, argv))
  {
    return EXIT_FAILURE;
  }

  // always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
  {
    vcl_cout << "problems in writing params file to: " 
      << params->print_params_file() << vcl_endl;
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


  if(vul_file_exists(params->get_output_folder()))
  {
      vul_file::make_directory(params->get_output_folder());
  }

  //>> Write input.xml file to output folder
  // filename format: input+applelogos_another.xml
  params->print_input_xml(params->get_output_folder() + "/" + "input+" + 
    params->input_object_name_() + ".xml");
  
  
  //////////////////////////////////////////////////////////////////////////////
  //2. Detect objects in image
  
  //> Make some announcements
  vcl_cout 
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
  vcl_vector<dbsks_det_desc_xgraph_sptr > det_list;

  // Unique id for pair of (prototype xgraph, image)
  vcl_string det_group_id = params->get_xgraph_prototype_name() + "+" + params->input_object_name_();

  // Path to folder where we will save detection results in
  vcl_string work_folder = params->get_output_folder() + "/" + det_group_id;

  //> Switch object-detection procedures
  int choice = 2;
  
  //b. Use edge map pyramid and subpixel ccm - take input as a sub-pixel edgemap
  if (choice == 2)
  {  
    //> create the work directory if not yet done
    if (!vul_file_exists(work_folder))
    {
      bool mkdir_status = vul_file::make_directory(work_folder);
      if(mkdir_status)
      {
          vcl_cout << "Work folder " << work_folder << " was successfully created!" << vcl_endl;
      }
      else
      {
          vcl_cout << "ERROR: Work folder " << work_folder << " was not created!" << vcl_endl;
          return EXIT_FAILURE;
      }
    }

    //> Set up params
    vox_detect_xgraph_using_pyramid_edgemap m;

    // object name
    m.object_id = params->input_object_name_();
    
    // image file
    m.image_file = params->get_image_file();

    // edgemap files
    m.edgemap_folder =  params->input_edgemap_folder_();
    m.edgemap_ext = params->input_edgemap_extension_();
    m.edgeorient_ext = params->input_edgeorient_extension_();
    m.edgemap_log2_scale_ratio = params->input_edgemap_log2_scale_ratio_();

    // xgraph protype + model
    m.xgraph_file =  params->get_xgraph_file();
    m.xgraph_geom_file = params->get_xgraph_geom_file();
    m.xgraph_ccm_file = params->get_xgraph_ccm_file();
    m.cfrag_list_to_ignore = params->get_cfrag_list_to_ignore();
    
    // xgraph scales
    m.prototype_xgraph_base_size = params->prototype_xgraph_base_size_();
    m.prototype_xgraph_min_size = params->prototype_xgraph_min_size_();
    m.prototype_xgraph_log2_increment_step = params->prototype_xgraph_log2_increment_step_();
    m.prototype_xgraph_max_size = params->prototype_xgraph_max_size_();
    m.prototype_xgraph_ratio_max_size_to_min_size = params->prototype_xgraph_ratio_max_size_to_min_size_();
    
    // detection wind
    m.det_window_width = params->detection_window_width_();
    m.det_window_height = params->detection_window_height_();

    // work folder
    m.work_folder = work_folder;

    // filtering output
    m.min_accepted_confidence = params->min_accepted_confidence_();
    m.run_nms_based_on_overlap = params->run_nms_based_on_overlap_();
    m.min_overlap_ratio_for_rejection = params->min_overlap_ratio_for_rejection_();

    //> Run detection
    bool success = m.execute();

    if (!success)
    {
      vcl_cout << "\nERROR: Xgraph detection failed. No results created." << vcl_endl;
      return EXIT_FAILURE;
    }

    //> Collect detection resulst
    det_list = m.output_det_list;
    image_resource = vil_load_image_resource(params->get_image_file().c_str());
  }
  else
  {
    vcl_cout << "\nERROR: Unknown choice." << vcl_endl;
    return EXIT_FAILURE;
  }

  //> Sort the detection results based in the order of decreasing confidence
  vcl_sort(det_list.begin(), det_list.end(), dbsks_decreasing_confidence);
  vcl_cout << "\n  Final number of detections= " << det_list.size() << vcl_endl;

  //////////////////////////////////////////////////////////////////////////////
  // 3. Save detection results

  vcl_cout
    << "\n---------------------------------------------------------------------"
    << "\nSave detection results"
    << "\n--------------------------------------------------------------------\n";

  //vcl_string output_file_prefix = params->get_output_folder() + "/";

  //> Save summary in VOC2008 format
  vcl_string det_result_file = params->get_output_folder() + "/" + 
    "VOC_det_result+" + params->get_xgraph_prototype_name() +
    "+" + params->input_object_name_() + ".txt";

  vcl_cout << "\n>> Save summary in VOC2008 format: " << det_result_file << "...";
  {
    // \todo turn this into a function
    vcl_ofstream ofs(det_result_file.c_str(), vcl_ios::out);
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
  vcl_cout << "done.\n";

  //> Save each detection: xgraph + screenshot
  vcl_cout << "\n>> Save each detected xgraph to output folder: " 
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
  vcl_cout << "done.\n";

  //////////////////////////////////////////////////////////////////////////////
  // 4. Evaluate detection results (compare against groundtruth)

  vcl_cout
    << "\n---------------------------------------------------------------------"
    << "\nEvaluate detection results"
    << "\n--------------------------------------------------------------------\n";
  
  //> Load groundtruth file
  vcl_string image_desc_file = params->input_object_dir_() + "/" + params->input_object_name_() + ".xml";

  vcl_cout << "\n>> Load groundtruth file: " << image_desc_file << "...";

  dborl_image_description_sptr image_desc;
  if (!x_read(image_desc_file, image_desc))
  {
    vcl_cout << "Failed.\n";
    return EXIT_FAILURE;
  }
  else
  {
    vcl_cout << "Done.\n";
  }

  //> Evaluate the detection results against objects in the image description
  vcl_cout << "\n>> Compare each detection against ground truth data ...";
  double min_required_overlap = 0.5; // PASCAL criteria
  vcl_vector<double > confidence_vec;
  vcl_vector<int > FP_vec;
  vcl_vector<int > TP_vec;
  int num_pos = -1;

  // Clone the detection list to its parent class
  vcl_vector<dborl_det_desc_sptr > borl_det_list;
  borl_det_list.reserve(det_list.size());
  for (unsigned i =0; i < det_list.size(); ++i)
  {
    borl_det_list.push_back(det_list[i].ptr());
  }
  dborl_VOC2008_eval_det(image_desc, params->input_model_category_(), min_required_overlap, borl_det_list,
    confidence_vec, FP_vec, TP_vec, num_pos);
  vcl_cout << "\nFinished comparing against ground truth.\n";

  //> Print out the evaluation results
  vcl_string eval_file = params->get_output_folder() + "/" + 
    "eval_det+" + params->get_xgraph_prototype_name() + "+" + 
    params->input_object_name_() + ".txt";

  vcl_cout << "\n>> Print out evaluation results to file: " << eval_file << vcl_endl;
  dborl_VOC2008_print_eval_results(eval_file, num_pos, params->input_object_name_(),
    params->input_model_category_(), confidence_vec, TP_vec, FP_vec);

  // Display total time spent
  vcl_cout << "\n\n>> Total time spent for the task: " 
    << timer.all() / 1000 << " seconds.\n" << vcl_endl;  
  
  return EXIT_SUCCESS;
};

