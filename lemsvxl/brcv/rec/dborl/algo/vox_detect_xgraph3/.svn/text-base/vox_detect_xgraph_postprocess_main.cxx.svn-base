// This example displays 

#include "vox_detect_xgraph_postprocess_utils.h"
#include <vul/vul_arg.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

// -----------------------------------------------------------------------------
//: Utilities to use with xgraph detection
int main(int argc, char **argv) 
{
  // Arguments
  vul_arg_info_list arg_list;
  
  // Re-compute cost of detection using WCM cost
  vul_arg<bool > recompute_cost_using_wcm(arg_list, "-recompute-cost-using-wcm",
    "Re-compute cost of detection using WCM cost", false);

  // Combine detection results from two folders - possibly using two different prototypes
  // or using different sets of scales
  vul_arg<bool > combine_det_results(arg_list, "-combine-det-results",
    "Combine detection results from multiple folders", false);
  
  // For each xshock detection record in .txt format, create an .xml file equivalent by adding the header and footer
  vul_arg<bool > convert_det_record_txt_to_xml(arg_list, "-convert-det-record-txt-to-xml",
    "Convert all xshock detection records from .txt to .xml format.", false);

  // Create a B/W screenshot of shape boundary for each xshock detection. Modify the det_record accordingly
  vul_arg<bool > create_bnd_screenshot(arg_list, "-create-bnd-screenshot",
    "Create B/W boundary screenshot for each detection. Modify detection record accordingly", false);

  // Create a B/W screenshot of shape boundary for each xshock detection. Modify the det_record accordingly
  vul_arg<bool > combine_det_records_per_image(arg_list, "-combine-det-records-per-image",
    "Combine all detection records for each image into one file", false);

  // Filter detection results using geometric constaints specified by geomtric model
  vul_arg<bool > filter_dets_using_geomtric_constraints(arg_list, "-filter-dets-using-geomtric-constraints",
    "filter dets using geomtric constraints", false);


  
  //----------------------------------------------------------------------------
  // Supporting arguments
  // List of xml detection record
  vul_arg<vcl_string > list_xml_det_record(arg_list, "-list-xml-det-record",
    "List of xml detection record (with no header and footer)", "");

  // Experiment folder
  vul_arg<vcl_string > exp_folder(arg_list, "-exp-folder",
    "Full path to the xshock detection experiment folder", "");

  // Output folder
  vul_arg<vcl_string > output_folder(arg_list, "-output-folder",
    "Full path to the output folder", "");

  vul_arg<vcl_string > xgraph_prototype_name(arg_list, "-xgraph-prototype-name",
    "Name of prototype to be part of the detection record file name");

  vul_arg<float > overlap_ratio(arg_list, "-overlap-ratio",
    "Ratio between areas of overlap and of union of two shapes");

  vul_arg<vcl_string > exp_name1(arg_list, "-exp-name1", "Name of experiment 1");
  vul_arg<vcl_string > exp_name2(arg_list, "-exp-name2", "Name of experiment 2");

  //----------------------------------------------------------------------------

  // print help
  vul_arg<bool > print_usage_only(arg_list,"-usage", "print usage info and exit", false);
  vul_arg<bool > print_help(arg_list,"-help", "print usage info and exit", false);

  // parse input arguments
  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);

  // Re-compute cost of detection using WCM cost
  if (recompute_cost_using_wcm())
  {
    return dbsks_recompute_cost_using_wcm() ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  // Combine detection results from 2 folders
  if (combine_det_results())
  {
    if (print_help())
    {
      vcl_cout << "\nCombine detection results from multiple experiments.\n";
      return EXIT_SUCCESS;
    }
    
    exp_folder() = "W:/xshock-experiments";
    exp_name1() = "exp_84-applelogos_prototype1-applelogos-v0_90-repeat_exp_82_long_range_ccm";
    exp_name2() = "";
    overlap_ratio() = 0.3f;
    output_folder() = "D:/vision/projects/symseg/xshock/xshock-experiments/exp_84b-reduce_overlap_ratio_0_3";
    xgraph_prototype_name() = "applelogos_prototype1";

    // execute the conversion
    return dbsks_combine_det_results(exp_folder(), exp_name1(), exp_name2(), 
      overlap_ratio(), output_folder(), xgraph_prototype_name()) 
      ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  
  // create .xml equivalence of xshock detection record in .txt format for a folder
  if (convert_det_record_txt_to_xml())
  {
    // check whether ONLY help info is needed
    if (print_help())
    {
      vcl_cout << "Usage with " << convert_det_record_txt_to_xml.option() << "option:\n"
        << exp_folder.option() << " " << exp_folder.help() << "\n";
      return EXIT_SUCCESS;
    }

    // execute the conversion
    return dbsks_convert_det_record_txt_to_xml(exp_folder()) ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  // Create a B/W screenshot of shape boundary for each xshock detection. Modify the det_record accordingly
  if (create_bnd_screenshot())
  {
    // check whether ONLY help info is needed
    if (print_help())
    {
      vcl_cout << "Usage with " << create_bnd_screenshot.option() << "option:\n"
        << exp_folder.option() << " " << exp_folder.help() << "\n";
      return EXIT_SUCCESS;
    }

    // execute the creation
    return dbsks_create_bnd_screenshot(exp_folder()) ? EXIT_SUCCESS : EXIT_FAILURE;
  }


  // combine detection records--------------------------------------------------
  if (combine_det_records_per_image())
  {
    // check whether ONLY help info is needed
    if (print_help())
    {
      vcl_cout << "Usage with " << combine_det_records_per_image.option() << "option:\n"
        << exp_folder.option() << " " << exp_folder.help() << "\n"
        << output_folder.option() << " " << output_folder.help() << "\n"
        << xgraph_prototype_name.option() << " " << xgraph_prototype_name.help() << "\n";
      return EXIT_SUCCESS;
    }

    // execute the creation
    return dbsks_combine_det_records_per_image(exp_folder(), output_folder(),
      xgraph_prototype_name()) ? EXIT_SUCCESS : EXIT_FAILURE;
  }

  // Filter detection results---------------------------------------------------
  // using geometric constaints specified by geomtric model
  if (filter_dets_using_geomtric_constraints())
  {
    // check whether ONLY help info is needed
    if (print_help())
    {
      vcl_cout << "Usage: \n";
      
      vcl_cout << "Good luck.\n";
        
      return EXIT_SUCCESS;
    }
    else
    {
      bool success = dbsks_filter_dets_using_geometric_constraints();

      return success ? EXIT_SUCCESS : EXIT_FAILURE;
    
    }
  }
    

  // Only print help
  if (print_help() || print_usage_only()) 
  {
    vul_arg_display_usage_and_exit();
    return EXIT_SUCCESS;
  }

  return 0;  
}












