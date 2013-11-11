// GUI menu for mleotta
#include "mleotta_menu.h"
#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <dbgui/dbgui_utils.h>

#include <bvis1/menu/bvis1_command.h>
#include <bvis1/menu/bvis1_file_menu.h>
#include <bvis1/menu/bvis1_video_menu.h>
#include <bvis1/menu/bvis1_process_macro_menu.h>
#include <bvis1/menu/bvis1_tableau_menu.h>
#include <bvis1/menu/bvis1_view_menu.h>
#include <bvis1/menu/bvis1_repository_menu.h>
#include <bvis1/menu/bvis1_add_menu.h>

#include <bvis1/bvis1_tool.h>
#include <bvis1/tool/bvis1_vsol2D_tools.h>
#include <bvis1/tool/bvis1_soview2D_tools.h>
#include <bvis1/tool/bvis1_correspond_tools.h>
#include <dbil/vis/dbil_image_intensity_inspector.h>
#include <dbil/vis/dbil_image_region_stats_tool.h>
//#include <dbmrf/vis/dbmrf_bmrf_tools.h>
#include <dbvrl/vis/dbvrl_region_tools.h>
#include <dbkpr/vis/dbkpr_keypoint_tools.h>
#include <dbecl/vis/dbecl_episeg_tools.h>
#include <dbetl/vis/dbetl_episeg_tools.h>
#include <dbbgm/vis/dbbgm_inspector_tool.h>
#ifdef HAS_BGUI3D
//#include <dbmrf/vis/dbmrf_curvel_3d_tools.h>
#endif

mleotta_menu::mleotta_menu() 
{
}

static void render_ps()
{
  dbgui_utils::render_to_ps(bvis1_manager::instance());
}

vgui_menu
mleotta_menu::setup_menu()
{
  vgui_menu MainMenu;
  
  /////////////// File Menu ///////////////
  bvis1_file_menu file_menu;

  //AMIR:added these to make loading and saving more intuitive
  vgui_menu load_menu;
  vgui_menu save_menu;

  //these have to be customized to each application which is why they cannot
  //be defined in bvis1_file_menu

  FILE_MENU_ADD_PROCESS_NAME( "Video", "Load Video", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( "Image", "Load Image", load_menu);
       MENU_ADD_PROCESS_NAME2( "Video", "Save Video", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Image", "Save Image", save_menu);
       MENU_ADD_PROCESS_NAME2( "Keypoints", "Save Keypoints", save_menu);

  file_menu.add("Load..", load_menu);
  file_menu.add("Save..", save_menu);
  file_menu.add("Save to PostScript", render_ps);
  file_menu.separator();

  file_menu.add_to_menu( MainMenu );

  
  /////////////// Video Menu ///////////////
  bvis1_video_menu  video_menu;
  video_menu.add_to_menu( MainMenu );

  
  /////////////// Repository Menu ////////////////////
  bvis1_repository_menu repository_menu;

  repository_menu.add_to_menu( MainMenu );

  
  /////////////// Add Menu ////////////////////
  bvis1_add_menu add_menu;

  add_menu.add_to_menu( MainMenu );


  /////////////// Processes Menu ///////////////
  vgui_menu processes_menu;

  vgui_menu  image_adjustment_group_menu; // Image Adjustment
    MENU_ADD_PROCESS_NAME( "Brightness/Contrast", image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Gaussian Blur",       image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Resample",            image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Convert to Grey",     image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Equalize Images",     image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Lens Distort Image",  image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Harr Wavelet",        image_adjustment_group_menu );

  vgui_menu  motion_detection_group_menu; // Motion Detection
    MENU_ADD_PROCESS_NAME( "Frame Difference",      motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Video Slice",           motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Motion",        motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Horn_Schunck",  motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Register Video",        motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Blob Tracker",          motion_detection_group_menu );

  vgui_menu  edge_detection_group_menu; // Edge Detection 
    MENU_ADD_PROCESS_NAME( "VD EdgeDetector",   edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Scale vsol",        edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Lens Distort VSOL", edge_detection_group_menu );

  vgui_menu  network_group_menu; // Network
    MENU_ADD_PROCESS_NAME( "Build MRF Network", network_group_menu );
    MENU_ADD_PROCESS_NAME( "Build 3D Curves",   network_group_menu );
    MENU_ADD_PROCESS_NAME( "Extract 2D Curves", network_group_menu );
    MENU_ADD_PROCESS_NAME( "Build Episegments", network_group_menu );
    MENU_ADD_PROCESS_NAME( "Track Epi-Points",  network_group_menu );
    MENU_ADD_PROCESS_NAME( "BMRF Predict",      network_group_menu );
    MENU_ADD_PROCESS_NAME( "BMRF Evaluate",     network_group_menu );
    MENU_ADD_PROCESS_NAME( "BMRF Back Project", network_group_menu );
    MENU_ADD_PROCESS_NAME( "Export ROC Data",   network_group_menu );

  vgui_menu  keypoints_group_menu; // Keypoints
    MENU_ADD_PROCESS_NAME( "Lowe Keypoints",         keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Match Keypoints",        keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Global Match Keypoints", keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Reconstruct Keypoints",  keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Depth Iterpolation",  keypoints_group_menu );

  vgui_menu  affine_patch_group_menu; // Affine Patch
    MENU_ADD_PROCESS_NAME( "Register Region", affine_patch_group_menu );
  
  vgui_menu  bg_model_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Init Background", bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Model Background", bg_model_group_menu );
//    MENU_ADD_PROCESS_NAME( "Joint Model Background", bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Truth BG Model",   bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Detect Background",   bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Detect Shadows",   bg_model_group_menu );
//    MENU_ADD_PROCESS_NAME( "Joint Detect Shadows",   bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "HMM Train Model",  bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "HMM Learn Transitions",  bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "HMM Detect",  bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Equalize with Background",  bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Position Model",  bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Model Classify",  bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Evaluate Segmentation",  bg_model_group_menu );
    
  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Harris Corners", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Image Stats", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Image Region Stats", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Scan Convert Polygons", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Model Depth Map", other_group_menu );
//    MENU_ADD_PROCESS_NAME( "Epipolar Param Image", other_group_menu );

  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Motion Detection", motion_detection_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
  processes_menu.add( "Network",          network_group_menu);
  processes_menu.add( "Keypoints",        keypoints_group_menu);
  processes_menu.add( "Affine Patch",     affine_patch_group_menu);
  processes_menu.add( "Background Model", bg_model_group_menu);
  processes_menu.add( "Other",            other_group_menu);

  MainMenu.add( "Processes" , processes_menu );


  /////////////// Process Macro Menu ///////////////

  bvis1_process_macro_menu  process_macro_menu;
  process_macro_menu.add_to_menu (MainMenu);


  /////////////// Tools Menu ///////////////
  vgui_menu tools;
  vgui_menu vsol2D_tools_menu, vsol2D_draw_tools_menu;
    MENU_ADD_TOOL( bvis1_vsol2D_line_tool,           vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_point_tool,          vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_polyline_tool,       vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_polygon_tool,        vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_digital_curve_tool,  vsol2D_draw_tools_menu );
    vsol2D_tools_menu.add("Drawing", vsol2D_draw_tools_menu);
    MENU_ADD_TOOL( bvis1_vsol2D_adjust_tool,         vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_split_curve_tool,    vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_correspond_point_tool,      vsol2D_tools_menu );
  
  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );
  
  vgui_menu image_tools_menu;
    MENU_ADD_TOOL( dbil_image_intensity_inspector, image_tools_menu );
    MENU_ADD_TOOL( dbil_image_region_stats_tool,   image_tools_menu );
    MENU_ADD_TOOL( dbbgm_inspector_tool,           image_tools_menu );
      
  vgui_menu keypoint_tools_menu;
    MENU_ADD_TOOL( dbkpr_keypoint_inspector_tool, keypoint_tools_menu );

  vgui_menu bmrf_tools_menu;
//    MENU_ADD_TOOL( dbmrf_inspector_tool,  bmrf_tools_menu );
//    MENU_ADD_TOOL( dbmrf_inspect_3d_tool, bmrf_tools_menu );
//    MENU_ADD_TOOL( dbmrf_xform_tool,      bmrf_tools_menu );
#ifdef HAS_BGUI3D
//    MENU_ADD_TOOL( dbmrf_curvel_3d_inspect_tool, bmrf_tools_menu );
#endif

  vgui_menu epi_tools_menu;
    MENU_ADD_TOOL( dbecl_episeg_inspector_tool,  epi_tools_menu );
    MENU_ADD_TOOL( dbetl_epiprofile_tool,        epi_tools_menu );

  vgui_menu bvrl_tools_menu;
    MENU_ADD_TOOL( dbvrl_region_transform_tool, bvrl_tools_menu );
    MENU_ADD_TOOL( dbvrl_region_super_res_tool, bvrl_tools_menu );
    
  tools.add("vsol2D",      vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("Image",       image_tools_menu);
  tools.add("Keypoint",    keypoint_tools_menu);
  tools.add("MRF Network", bmrf_tools_menu);
  tools.add("Epipolar",    epi_tools_menu);
  tools.add("Registration", bvrl_tools_menu);

  MainMenu.add( "Tools" , tools );


  /////////////// Tableau Menu ///////////////
  bvis1_tableau_menu  tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  
  /////////////// View Menu ////////////////
  bvis1_view_menu view_menu;
  view_menu.add_to_menu(MainMenu);

  return MainMenu;
}


