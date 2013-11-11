// This example displays 

#include <vgui/vgui.h> 
#include <vgui/vgui_easy2D_tableau.h> 
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>


#include "dbsks_cmdline_detect_utils.h"

#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/vis/dbsksp_shock_displayer.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <bgui/bgui_selector_tableau.h>
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vcl_cstdlib.h>


//: Utilities to use with xgraph detection
int main(int argc, char **argv) 
{
  // Arguments
  vul_arg_info_list arg_list;
  
   // Use a shape fragment model and apply it to an image to detect the positive fragment
  vul_arg<bool > extend_giraffe_legs(arg_list, "-extend-giraffe-legs",
    "Extend the front and rear legs of the giraffe models", false);

  // Re-compute bounding boxes of detections that were wrongly computed and saved
  vul_arg<bool > recompute_detection_bbox(arg_list, "-recompute-detection-bbox",
    "Re-compute bounding boxes of detections that were wrongly computed and saved", false);

  // print help
  vul_arg<bool > print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool > print_help(arg_list,"-help", "print usage info and exit",false);

  // parse input arguments
  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);

  // Only print help
  if (print_help() || print_usage_only()) 
  {
    vul_arg_display_usage_and_exit();
    return 0;
  }


  //// extend legs of giraffe models
  //if (extend_giraffe_legs())
  //{
  //  return dbsks_extend_giraffe_legs();
  //}


  //// recompute bounding boxes of detections that were wrongly computed and saved
  //else if (recompute_detection_bbox())
  //{
  //  return dbsks_recompute_detection_bbox();
  //}
  return 0;  
}












