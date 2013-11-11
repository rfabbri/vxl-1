#include "mw_stereo_menu.h"
#include <mw/app/mw_app.h>
#include <mw/app/mw_load_data.h>

#include <bvis1/bvis1_manager.h>
#include <bvis1/menu/bvis1_command.h> 
#include <bvis1/menu/bvis1_file_menu.h>
#include <bvis1/menu/bvis1_view_menu.h>
#include <bvis1/menu/bvis1_video_menu.h>
#include <bvis1/menu/bvis1_repository_menu.h>
#include <bvis1/menu/bvis1_add_menu.h>
#include <bvis1/menu/bvis1_process_macro_menu.h>
#include <bvis1/menu/bvis1_tableau_menu.h>
#include <bvis1/bvis1_tool.h>
#include <bvis1/tool/bvis1_vsol2D_tools.h>
#include <bvis1/tool/bvis1_soview2D_tools.h>
#include <bvis1/tool/bvis1_correspond_tools.h>

#include <dbdet/vis/dbdet_query_linking_tool.h>
#include <dbdet/vis/dbdet_sel_explorer_tool.h>

#include <dvpgl/vis/dvpgl_epipolar_tool.h>
#include <dbkpr/vis/dbkpr_keypoint_tools.h>
#include <dbdet/vis/dbdet_crop_edgemap_tool.h>


#include <dvpgl/vis/dvpgl_epipolar_region_tool.h>
#include <mw/vis/mw_curve_tracing_tool_2.h>
#include <mw/vis/mw_curve_dt_tracing_tool_2.h>
#include <mw/vis/mw_curve_edit_tracing_tool_2.h>
//#include <mw/vis/mw_curve_tracing_tool.h>
#include <mw/vis/mw_curve_appearance_tracing_tool.h>
//#include <mw/vis/mw_curve_dt_tracing_tool.h>
#include <mw/vis/oldtracer/mw_curve_tracing_tool_3.h>
//#include <mw/vis/mw_sel_tracing_tool.h>
#include <mw/vis/mw_correspond_point_tool.h>
#include <mw/vis/mw_3_correspond_point_tool.h>
#include <mw/vis/mw_correspond_point_tool_basic.h>
#include <mw/vis/mw_correspond_point_tool_band.h>
#include <mw/vis/mw_sel_inliers_to_curve.h>
//#include <mw/vis/mw_sel_geometry_tool.h>


#include <vcl_iostream.h>

mw_stereo_menu::mw_stereo_menu()
{
}

vgui_menu
mw_stereo_menu::setup_menu()
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
  FILE_MENU_ADD_PROCESS_NAME( ".EDG", "Load .EDG File", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Load .CEM File", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".CEMv", "Load .CEM File (vsol)", load_menu );
  MENU_ADD_PROCESS_NAME2( ".CVLET", "Load .CVLET File", load_menu);

  MENU_ADD_PROCESS_NAME("Load camera file", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( "MW Corresp (vsl)", "Load correspondences", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Vsol storage (vsl)", "Load vsol storage", load_menu);

  MENU_ADD_PROCESS_NAME2( "Video", "Save Video", save_menu);
  MENU_ADD_PROCESS_NAME2( ".EDG", "Save .EDG File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Save .CEM File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CEMv", "Save .CEM File (vsol)", save_menu);
  MENU_ADD_PROCESS_NAME2( ".CVLET", "Save .CVLET File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Vsol storage (vsl)", "Save vsol storage", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "MW Corresp (vsl)", "Save correspondences", save_menu);


  file_menu.add("Load..", load_menu);
  file_menu.add("Save..", save_menu);
  file_menu.separator();

  file_menu.add_to_menu( MainMenu );

  /////////////// Video Menu ///////////////
  bvis1_video_menu video_menu;
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
    MENU_ADD_PROCESS_NAME( "Convert to Grey",     image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Resample",            image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Lens Distort Image",  image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Gradients",   image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Distance Transform",   image_adjustment_group_menu );

  vgui_menu  edge_detection_group_menu; // Edge Detection
    MENU_ADD_PROCESS_NAME( "VD EdgeDetector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Generic Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Third Order Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "RGB Composite Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Generic Color Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Third Order Color Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Generic Multiscale Edge Detector", edge_detection_group_menu );
    edge_detection_group_menu.separator();
    MENU_ADD_PROCESS_NAME( "Compute Topographic Curves", edge_detection_group_menu );
    edge_detection_group_menu.separator();
    MENU_ADD_PROCESS_NAME( "Generic Edge Linker", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Symbolic Edge Linker", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Extract Linked Curves", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "All in one Edge > Sel > Vsol", edge_detection_group_menu );
    
    edge_detection_group_menu.separator();
    
    MENU_ADD_PROCESS_NAME( "Prune Edges",  edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Curve Fragments",  edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Smooth Edges", edge_detection_group_menu);
    MENU_ADD_PROCESS_NAME( "Smooth Contours", edge_detection_group_menu);
    MENU_ADD_PROCESS_NAME( "Convert Edgemap to Image", edge_detection_group_menu );
    
    edge_detection_group_menu.separator();

    MENU_ADD_PROCESS_NAME( "Contour Tracer", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Lens Distort VSOL", edge_detection_group_menu );
    edge_detection_group_menu.separator();
    MENU_ADD_PROCESS_NAME( "Combine VSOL from Frames", edge_detection_group_menu );

  vgui_menu  keypoints_group_menu; // Keypoints
    MENU_ADD_PROCESS_NAME( "Lowe Keypoints",         keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Match Keypoints",        keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Global Match Keypoints", keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Reconstruct Keypoints",  keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Reconstruct Frenet Keypoints",  keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Projective-Reconstruct Keypoints", keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Depth Iterpolation",  keypoints_group_menu );
    MENU_ADD_PROCESS_NAME( "Harris Corners", keypoints_group_menu);

  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Sample Function", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Edgel Test Data", other_group_menu);
    MENU_ADD_PROCESS_NAME( "Test SEL Geometry - Circle", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Synthetic Correspondences", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Smooth Contours", other_group_menu );

  vgui_menu  multiview_group_menu; // Multiple view geometry
    MENU_ADD_PROCESS_NAME( "Project Cube", multiview_group_menu);
    MENU_ADD_PROCESS_NAME( "Project Polyline", multiview_group_menu);
    MENU_ADD_PROCESS_NAME( "Project Curve Sketch", multiview_group_menu);
    MENU_ADD_PROCESS_NAME( "Extend MW Corresp", multiview_group_menu);
    MENU_ADD_PROCESS_NAME( "Nearest Edgels to Polyline", multiview_group_menu);

  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
  processes_menu.add( "Multiview",        multiview_group_menu);
  processes_menu.add( "Keypoints",        keypoints_group_menu);
  processes_menu.add( "Other",            other_group_menu);

  MainMenu.add( "Processes" , processes_menu );
  

  /////////////// Process Macro Menu ///////////////
  bvis1_process_macro_menu process_macro_menu;
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
    MENU_ADD_TOOL( bvis1_vsol2D_split_curve_tool,    vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_inspect_tool,        vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_adjust_tool,         vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_crop_tool,           vsol2D_tools_menu );
    vsol2D_draw_tools_menu.separator();
    MENU_ADD_TOOL( bvis1_correspond_point_tool,      vsol2D_tools_menu );

  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );

//  vgui_menu image_tools_menu;
//    MENU_ADD_TOOL( dbil_image_intensity_inspector, image_tools_menu );

  vgui_menu keypoint_tools_menu;
    MENU_ADD_TOOL( dbkpr_keypoint_inspector_tool, keypoint_tools_menu );

  vgui_menu edge_linking_tools_menu;
    MENU_ADD_TOOL( dbdet_crop_edgemap_tool,      edge_linking_tools_menu );
    MENU_ADD_TOOL( dbdet_query_linking_tool,     edge_linking_tools_menu );
    MENU_ADD_TOOL( dbdet_sel_explorer_tool,      edge_linking_tools_menu );
//    MENU_ADD_TOOL( mw_sel_geometry_tool,         edge_linking_tools_menu );

  vgui_menu epi_tools_menu;
    // tool to display epipolar lines:
    MENU_ADD_TOOL( dvpgl_epipolar_tool,          epi_tools_menu);
    MENU_ADD_TOOL( dvpgl_epipolar_region_tool,   epi_tools_menu);

  tools.add("vsol2D",      vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("Epipolar",    epi_tools_menu);
  tools.add("Edge Linking", edge_linking_tools_menu);
//  tools.add("Image",       image_tools_menu);
  tools.add("Keypoint",    keypoint_tools_menu);


  MENU_ADD_TOOL( mw_curve_tracing_tool_2, tools);
  MENU_ADD_TOOL( mw_curve_dt_tracing_tool_2, tools);
  MENU_ADD_TOOL( mw_curve_edit_tracing_tool_2, tools);
  MENU_ADD_TOOL( mw_curve_tracing_tool_3, tools);
//  MENU_ADD_TOOL( mw_curve_tracing_tool, tools);
  MENU_ADD_TOOL( mw_curve_appearance_tracing_tool, tools);
//  MENU_ADD_TOOL( mw_curve_dt_tracing_tool, tools);
//  MENU_ADD_TOOL( mw_sel_tracing_tool, tools);
//  MENU_ADD_TOOL( mw_correspond_point_tool, tools);
  MENU_ADD_TOOL( mw_3_correspond_point_tool, tools);
  MENU_ADD_TOOL( mw_correspond_point_tool_band, tools);
  MENU_ADD_TOOL( mw_correspond_point_tool_basic, tools);
  tools.separator();
  MENU_ADD_TOOL( mw_sel_inliers_to_curve , tools);

  MainMenu.add( "Tools", tools );

  /////////////// Tableau Menu ///////////////
  bvis1_tableau_menu tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  /////////////// View Menu ////////////////
  bvis1_view_menu view_menu;
  view_menu.add_to_menu(MainMenu);
  
  ////////// Apps menu ////////////

  vgui_menu apps_menu;

  apps_menu.separator();
  apps_menu.add("Working state",mw_load_current_working_repository);
  apps_menu.add("Load mcs_instance",mw_load_mcs_instance);
  apps_menu.separator();
  MENU_ADD_PROCESS_NAME("Show Contours", apps_menu);
  apps_menu.add("Example",mw_misc);
  apps_menu.add("Load CT dataset",load_ct_spheres_dataset);
  apps_menu.add("Binocular Analytic dataset",mw_load_data::load_rotational_analytic_pair);
  apps_menu.add("Analytic dataset ctspheres",mw_load_data::load_rotational_analytic_views);
  apps_menu.add("Analytic dataset olympus",mw_load_data::load_rotational_analytic_views_olympus_turntable);

  MainMenu.add("Apps",apps_menu);

  return MainMenu;
}
