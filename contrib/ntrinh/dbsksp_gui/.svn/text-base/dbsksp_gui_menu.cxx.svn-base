// GUI menu for dbsksp_gui
#include "dbsksp_gui_menu.h"
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
#include <dbsol/vis/dbsol_cut_contour_tool.h>

#include <dbsk2d/vis/dbsk2d_ishock_getinfo_tool.h>
#include <dbsk2d/vis/dbsk2d_ishock_detector_debug_tool.h>
#include <dbsk2d/vis/dbsk2d_ishock_splice_transform_tool.h>
#include <dbsk2d/vis/dbsk2d_ishock_gap_transform_tool.h>
#include <dbsk2d/vis/dbsk2d_ishock_loop_transform_tool.h>
#include <dbsk2d/vis/dbsk2d_rich_map_query_tool.h>
#include <dbsk2d/vis/dbsk2d_shock_image_fragment_tool.h>

#include <dbsksp/vis/dbsksp_shock_design_tool.h>
#include <dbsksp/vis/dbsksp_xshock_design_tool.h>
#include <dbsksp/vis/dbsksp_interp_two_xnodes_tool.h>
#include <dbsksp/vis/dbsksp_shock_snake_tool.h>

#include <dbsks/vis/dbsks_examine_graph_cost_tool.h>


dbsksp_gui_menu::dbsksp_gui_menu() 
{
}

static void render_ps()
{
  dbgui_utils::render_to_ps(bvis1_manager::instance());
}

vgui_menu
dbsksp_gui_menu::setup_menu()
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
  FILE_MENU_ADD_PROCESS_NAME( "Image Sequence", "Load Image Sequence", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Load .bnd File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".ESF", "Load .ESF File", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".CON", "Load .CON File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".EDG", "Load .EDG File", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Load .CEM File", load_menu );

  //FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Load .CEM File (vsol)", load_menu );
  
  FILE_MENU_ADD_PROCESS_NAME( "shock graph XML", "Load shock graph XML", load_menu);

  FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Save .bnd File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".ESF", "Save .ESF File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Save .CEM File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CON", "Save .CON File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Image", "Save Image", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".PS", "Save .PS File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "shock graph XML", "Save shock graph XML", save_menu);
  


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
    MENU_ADD_PROCESS_NAME( "Convert to Grey",     image_adjustment_group_menu );

  vgui_menu  motion_detection_group_menu; // Motion Detection
    MENU_ADD_PROCESS_NAME( "Frame Difference", motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Motion",   motion_detection_group_menu );

  vgui_menu  edge_detection_group_menu; // Edge Detection
    MENU_ADD_PROCESS_NAME( "VD EdgeDetector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Generic Edge Detector", edge_detection_group_menu );
    //MENU_ADD_PROCESS_NAME( "Logical Linear",  edge_detection_group_menu );
    //MENU_ADD_PROCESS_NAME( "Susan",           edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Generic Color Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Third Order Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Third Order Color Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Generic Edge Linker", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Curve Fragments", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Contour Tracer", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Edges",  edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Symbolic Edge Linker", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Smooth Contours", edge_detection_group_menu);
    MENU_ADD_PROCESS_NAME( "Compute Gradients", edge_detection_group_menu);
    MENU_ADD_PROCESS_NAME( "Convert Edgemap to Image", edge_detection_group_menu);
    MENU_ADD_PROCESS_NAME( "Extract Linked Curves", edge_detection_group_menu);
  
  vgui_menu  transform_group_menu; // Transformation
    MENU_ADD_PROCESS_NAME( "Translate vsol2D objects", transform_group_menu );
    MENU_ADD_PROCESS_NAME( "ROI Image", transform_group_menu );
    MENU_ADD_PROCESS_NAME( "Fit circular arc spline", transform_group_menu );

  vgui_menu  curve_group_menu; // Curve
    MENU_ADD_PROCESS_NAME( "Curve Matching", curve_group_menu );
    MENU_ADD_PROCESS_NAME( "Interpolated Curve Matching", curve_group_menu );

  vgui_menu  shock_group_menu; // Shock
    MENU_ADD_PROCESS_NAME( "Compute Shocks", shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Shocks",   shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Perform Gap Transforms",   shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Sample Shocks",  shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Compile Rich Map",  shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Assign an Image",  shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Shock-path Curve",  shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Match Shock Graphs", shock_group_menu );
    //MENU_ADD_PROCESS_NAME( "Match Shock Graphs using MI", shock_group_menu );





  vgui_menu  shockshape_group_menu; // symmetry-shape
    MENU_ADD_PROCESS_NAME( "Build Shock Graph", shockshape_group_menu );
    MENU_ADD_PROCESS_NAME( "Build from extrinsic shock graph", shockshape_group_menu );
    
    MENU_ADD_PROCESS_NAME( "Shock snake", shockshape_group_menu );
    MENU_ADD_PROCESS_NAME( "Deformable Shock", shockshape_group_menu );
    MENU_ADD_PROCESS_NAME( "Trace shock boundary", shockshape_group_menu );
    MENU_ADD_PROCESS_NAME( "Morph Shock Graph", shockshape_group_menu );
    MENU_ADD_PROCESS_NAME( "Morph Shock Graph - different topology", shockshape_group_menu );
    MENU_ADD_PROCESS_NAME( "Convert shock graph to xshock graph", shockshape_group_menu );
    MENU_ADD_PROCESS_NAME( "Convert sk2d graph to xshock graph", shockshape_group_menu );
    MENU_ADD_PROCESS_NAME( "Simplify xshock graph", shockshape_group_menu );

  vgui_menu  segmentation_group_menu; // symmetry-shape
    MENU_ADD_PROCESS_NAME( "Compute Symmetry Points", segmentation_group_menu );
    MENU_ADD_PROCESS_NAME( "Detect Shape", segmentation_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute arc cost", segmentation_group_menu );
    MENU_ADD_PROCESS_NAME( "Save arc cost", segmentation_group_menu );
    MENU_ADD_PROCESS_NAME( "Load arc cost", segmentation_group_menu );
    MENU_ADD_PROCESS_NAME( "Save DP results", segmentation_group_menu );
    MENU_ADD_PROCESS_NAME( "Load DP results", segmentation_group_menu );
    MENU_ADD_PROCESS_NAME( "Load shock graph XML", segmentation_group_menu);
    MENU_ADD_PROCESS_NAME( "Trace opt graph boundary", segmentation_group_menu);
    MENU_ADD_PROCESS_NAME( "Local Match", segmentation_group_menu);
    MENU_ADD_PROCESS_NAME( "Write shapematch to PS", segmentation_group_menu);
    MENU_ADD_PROCESS_NAME( "Compute shock graph statistics", segmentation_group_menu);


  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Harris Corners", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Save vsol2D to Postscript File", other_group_menu );
    
  

  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Motion Detection", motion_detection_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
  processes_menu.add( "vsol2D transform", transform_group_menu );
  processes_menu.add( "Curve",            curve_group_menu );
  processes_menu.add( "Shock",            shock_group_menu );
  processes_menu.add( "Shape Modelling",  shockshape_group_menu );
  processes_menu.add( "Segmentation",     segmentation_group_menu );

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
    MENU_ADD_TOOL( bvis1_vsol2D_split_curve_tool,    vsol2D_tools_menu );
  
  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );

  // vsol 2D tools
  MENU_ADD_TOOL( dbsol_cut_contour_tool,             vsol2D_tools_menu );



  vgui_menu shock_tools_menu;
    MENU_ADD_TOOL( dbsk2d_ishock_getinfo_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_ishock_detector_debug_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_ishock_splice_transform_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_ishock_gap_transform_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_ishock_loop_transform_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_rich_map_query_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_shock_image_fragment_tool, shock_tools_menu );




  vgui_menu dbsksp_tools_menu;
    MENU_ADD_TOOL( dbsksp_shock_design_tool, dbsksp_tools_menu );
    MENU_ADD_TOOL( dbsksp_xshock_design_tool, dbsksp_tools_menu );
    MENU_ADD_TOOL( dbsksp_interp_two_xnodes_tool, dbsksp_tools_menu );
    MENU_ADD_TOOL( dbsksp_shock_snake_tool, dbsksp_tools_menu );

  vgui_menu dbsks_tools_menu;
    MENU_ADD_TOOL( dbsks_examine_graph_cost_tool, dbsks_tools_menu );
    
  tools.add("vsol2D", vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("Shock",       shock_tools_menu);
  tools.add("Shape Modeling", dbsksp_tools_menu);
  tools.add("Symmetry-Based Segmentation", dbsks_tools_menu);


  MainMenu.add( "Tools" , tools );


  /////////////// Tableau Menu ///////////////
  bvis1_tableau_menu  tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  
  /////////////// View Menu ////////////////
  bvis1_view_menu view_menu;
  view_menu.add_to_menu(MainMenu);

  return MainMenu;
}


