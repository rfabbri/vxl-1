//This is contrib/shock2d/gui/shock2d_gui_menu.cxx

#include "shock2d_gui_menu.h"

#include <bvis1/bvis1_manager.h>
#include <bvis1/menu/bvis1_command.h>

#include <vidpro1/vidpro1_process_manager.h>

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

#include <dbsk2d/vis/dbsk2d_ishock_getinfo_tool.h>
#include <dbsk2d/vis/dbsk2d_ishock_detector_debug_tool.h>
#include <dbsk2d/vis/dbsk2d_ishock_splice_transform_tool.h>
#include <dbsk2d/vis/dbsk2d_ishock_gap_transform_tool.h>
#include <dbsk2d/vis/dbsk2d_ishock_loop_transform_tool.h>
#include <dbsk2d/vis/dbsk2d_shock_subgraph_tool.h>

#include <dbsk2d/vis/dbsk2d_rich_map_query_tool.h>
#include <dbsk2d/vis/dbsk2d_shock_image_fragment_tool.h>

#include <dbskr/vis/dbskr_debug_scurve_tool.h>
#include <dbskr/vis/dbskr_subgraph_scurve_tool.h>

#include <dbru/vis/dbru_rcor_tool.h>
#include <dbru/vis/dbru_matching_tool.h>
#include <dber/vis/dber_instance_tools.h>

#include <dbdet/vis/dbdet_livewire_tool.h>

shock2d_gui_menu::shock2d_gui_menu()
{
}

vgui_menu
shock2d_gui_menu::setup_menu()
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
  FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Load .bnd File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CON", "Load .CON File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".EDG", "Load .EDG File", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Load .CEM File (vsol)", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".ESF", "Load .ESF File", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".DAT", "Load Video Instances", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".XML", "Load Composite Graph", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".XML", "Load Training File", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".BIN", "Load Binary Composite Graph", load_menu );

  FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Save .bnd File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Save .CEM File (vsol)", save_menu);
  MENU_ADD_PROCESS_NAME2( ".CON", "Save .CON File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".ESF", "Save .ESF File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".DAT", "Save Video Instances", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".XML", "Save Composite Graph", save_menu );

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
    MENU_ADD_PROCESS_NAME( "RGB to IHS",          image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Watershed Transform 2D",  image_adjustment_group_menu );

  vgui_menu  motion_detection_group_menu; // Motion Detection
    MENU_ADD_PROCESS_NAME( "Frame Difference", motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Background Difference", motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Motion", motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Register Video", motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "dbinfo_multi_track_process", motion_detection_group_menu );

  vgui_menu  edge_detection_group_menu; // Edge Detection
    MENU_ADD_PROCESS_NAME( "VD EdgeDetector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Generic Edge Detector", edge_detection_group_menu );
    //MENU_ADD_PROCESS_NAME( "Logical Linear",  edge_detection_group_menu );
    //MENU_ADD_PROCESS_NAME( "Susan",           edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Generic Color Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Third Order Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Third Order Color Edge Detector", edge_detection_group_menu );
    //MENU_ADD_PROCESS_NAME( "Superimpose Registered Frames", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Symbolic Edge Linker", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Curve Fragments", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Contour Tracer", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Edges",  edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Mask Edges",  edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Smooth Contours", edge_detection_group_menu);
    MENU_ADD_PROCESS_NAME( "Match Edgel Sets", edge_detection_group_menu);
    MENU_ADD_PROCESS_NAME( "Match Edgel Sets Video", edge_detection_group_menu);

  vgui_menu  shock_group_menu; // Shock
    MENU_ADD_PROCESS_NAME( "Compute Shocks", shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Shocks",   shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Perform Gap Transforms",   shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Sample Shocks",  shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Compile Rich Map",  shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Assign an Image",  shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Avg. Intenisty Image",  shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Shock-path Curve",  shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Match Shock Graphs", shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Match Shock Graphs using MI", shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Composite Graph", shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Detect Composite Transforms", shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Apply Composite Transforms",shock_group_menu);
    MENU_ADD_PROCESS_NAME( "Match Composite Graphs",shock_group_menu);
    MENU_ADD_PROCESS_NAME( "Extract Fragments",shock_group_menu);
    MENU_ADD_PROCESS_NAME( "Prune Fragments",shock_group_menu);
    MENU_ADD_PROCESS_NAME( "Match Bag of Fragments",shock_group_menu);
    MENU_ADD_PROCESS_NAME( "Detect Composite Fragments",shock_group_menu);          MENU_ADD_PROCESS_NAME( "Compute Containment Graph",shock_group_menu);
    MENU_ADD_PROCESS_NAME("Compute Containment Graph From Region",
                       shock_group_menu);

  vgui_menu  patch_group_menu; // Shock Patches
    MENU_ADD_PROCESS_NAME( "Match Shock Subgraphs", patch_group_menu );
    MENU_ADD_PROCESS_NAME( "Match Shock Patches", patch_group_menu );
    MENU_ADD_PROCESS_NAME( "Detect Shock Patches", patch_group_menu );
    MENU_ADD_PROCESS_NAME( "Extract Shock Patches", patch_group_menu );
    MENU_ADD_PROCESS_NAME( "Save Shock Patch Storage", patch_group_menu );
    MENU_ADD_PROCESS_NAME( "Load Shock Patch Storage", patch_group_menu );
    MENU_ADD_PROCESS_NAME( "Extract Subgraph and Find Shock Patches", patch_group_menu );

  vgui_menu  curve_group_menu; // Curve
    //MENU_ADD_PROCESS_NAME( "Curve Matching", curve_group_menu );
    //MENU_ADD_PROCESS_NAME( "Graduated Assignment", curve_group_menu );
    MENU_ADD_PROCESS_NAME( "Curve Distance Map", curve_group_menu );
    MENU_ADD_PROCESS_NAME( "Observation Info Matching", curve_group_menu );
    //MENU_ADD_PROCESS_NAME( "Shock Extraction and Observation Info Matching", shock_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Region Correspondence", curve_group_menu );
    MENU_ADD_PROCESS_NAME( "Print Region Correspondence", curve_group_menu );
    MENU_ADD_PROCESS_NAME( "Create random segments", curve_group_menu );

  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Harris Corners", other_group_menu );


  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Motion Detection", motion_detection_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
  processes_menu.add( "Shock",            shock_group_menu );
  processes_menu.add( "Shock Patches",    patch_group_menu );
  processes_menu.add( "Curve",            curve_group_menu );
  processes_menu.add( "Other",            other_group_menu);
 

  MainMenu.add( "Processes" , processes_menu );
  

  /////////////// Process Macro Menu ///////////////
  bvis1_process_macro_menu process_macro_menu;
  process_macro_menu.add_to_menu (MainMenu);

  /////////////// Tools Menu ///////////////
  vgui_menu tools;
   
  vgui_menu image_tools_menu;
  //MENU_ADD_TOOL(dbvis1_image_intensity_inspector, image_tools_menu );

  vgui_menu vsol2D_tools_menu, vsol2D_draw_tools_menu;
    MENU_ADD_TOOL( bvis1_vsol2D_line_tool,           vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_point_tool,          vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_polyline_tool,       vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_polygon_tool,        vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_digital_curve_tool,  vsol2D_draw_tools_menu );

  vsol2D_tools_menu.add("Drawing", vsol2D_draw_tools_menu);
    MENU_ADD_TOOL( bvis1_vsol2D_split_curve_tool,    vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_crop_tool,           vsol2D_tools_menu);
    MENU_ADD_TOOL( bvis1_vsol2D_selection_tool,      vsol2D_tools_menu);
    //MENU_ADD_TOOL( bvis1_vsol2D_rotate_line_tool,    vsol2D_tools_menu );
    //MENU_ADD_TOOL( bvis1_vsol2D_intersect_curve_tool,vsol2D_tools_menu );
    //MENU_ADD_TOOL( bvis1_correspond_point_tool,      vsol2D_tools_menu );
    //MENU_ADD_TOOL( dbvis1_edge_selection_tool,       vsol2D_tools_menu );
    //MENU_ADD_TOOL( dbvis1_edge_transform_tool,       vsol2D_tools_menu );
    //MENU_ADD_TOOL( dbvis1_classify_curve_tool,       vsol2D_tools_menu );

  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );

  vgui_menu shock_tools_menu;
    MENU_ADD_TOOL( dbsk2d_ishock_getinfo_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_ishock_detector_debug_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_ishock_splice_transform_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_ishock_gap_transform_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_ishock_loop_transform_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_rich_map_query_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbsk2d_shock_image_fragment_tool, shock_tools_menu );
    MENU_ADD_TOOL( dbskr_debug_scurve_tool, shock_tools_menu );
    vgui_menu patch_tools_menu;
    MENU_ADD_TOOL( dbsk2d_shock_subgraph_tool, patch_tools_menu );
    MENU_ADD_TOOL( dbskr_subgraph_scurve_tool, patch_tools_menu );

  vgui_menu lvwr_tools_menu;
    MENU_ADD_TOOL( dbdet_livewire_tool, lvwr_tools_menu );
    
  // vgui_menu shockshape_tools_menu;
  //  MENU_ADD_TOOL( dbvis1_shockshape_tool, shockshape_tools_menu );
  //  MENU_ADD_TOOL( dbvis1_triangularblob_tool, shockshape_tools_menu );

  vgui_menu rcor_tools_menu;
    MENU_ADD_TOOL( dbru_rcor_tool, rcor_tools_menu );
    MENU_ADD_TOOL( dbru_matching_tool, rcor_tools_menu );
    MENU_ADD_TOOL( dber_instance_label_tool, rcor_tools_menu );

  tools.add("vsol2D",      vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("Shock",       shock_tools_menu);
  tools.add("Shock patches", patch_tools_menu);
  tools.add("Livewire", lvwr_tools_menu);
  // tools.add("Shape Modeling", shockshape_tools_menu);
  tools.add("Region Corr.", rcor_tools_menu);

  MainMenu.add( "Tools", tools );

  /////////////// Tableau Menu ///////////////
  bvis1_tableau_menu tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  /////////////// Batch Menu ///////////////
  //dbvis1_batch_menu batch_menu;
  //batch_menu.add_to_menu( MainMenu );

  /////////////// View Menu ////////////////
  bvis1_view_menu view_menu;
  view_menu.add_to_menu(MainMenu);

  return MainMenu;
}
