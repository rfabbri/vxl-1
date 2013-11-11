//This is contrib/shock2d/gui/sel_gui_menu.cxx

#include "sel_gui_menu.h"

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

#include <dbdet/vis/dbdet_crop_edgemap_tool.h>
#include <dbdet/vis/dbdet_query_linking_tool.h>
#include <dbdet/vis/dbdet_sel_explorer_tool.h>
#include <dbdet/vis/dbdet_sel_contour_explorer_tool.h>
#include <dbdet/vis/dbdet_sel_edge_linking_tool.h>
#include <dbdet/vis/dbdet_sel_curvelet_linking_tool.h>
#include <dbdet/vis/dbdet_crop_curvelet_map_tool.h>
#include <dbdet/vis/dbdet_sel_CC_linking_tool.h>
#include <dbdet/vis/dbdet_sel_DHT_tool.h>

sel_gui_menu::sel_gui_menu()
{
}


vgui_menu
sel_gui_menu::setup_menu()
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
  //FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Load .bnd File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CON", "Load .CON File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".EDG", "Load .EDG File", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Load .CEM File", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".CEMv", "Load .CEM File (vsol)", load_menu );
  MENU_ADD_PROCESS_NAME2( ".CVLET", "Load .CVLET File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Vsol storage (vsl)", "Load vsol storage", load_menu);

  //FILE_MENU_ADD_PROCESS_NAME( ".ESF", "Load .ESF File", load_menu );
  

  MENU_ADD_PROCESS_NAME2( "Video", "Save Video", save_menu );
  FILE_MENU_ADD_PROCESS_NAME( "Image", "Save Image", save_menu);
  //FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Save .bnd File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Save .CEM File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CEMv", "Save .CEM File (vsol)", save_menu);
  MENU_ADD_PROCESS_NAME2( ".CON", "Save .CON File", save_menu);
  MENU_ADD_PROCESS_NAME2( ".EDG", "Save .EDG File", save_menu);
  MENU_ADD_PROCESS_NAME2( ".CVLET", "Save .CVLET File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Vsol storage (vsl)", "Save vsol storage", save_menu);

  //FILE_MENU_ADD_PROCESS_NAME( ".ESF", "Save .ESF File", save_menu);

  file_menu.add("Load..", load_menu);
  file_menu.add("Save..", save_menu);
  file_menu.separator();

  file_menu.add("Export to PS...", new sel_print_command());
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
    MENU_ADD_PROCESS_NAME( "Resample",            image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Brightness/Contrast", image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Gaussian Blur",       image_adjustment_group_menu );

    image_adjustment_group_menu.separator();

    MENU_ADD_PROCESS_NAME( "Convert to Grey",     image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "RGB to IHS",          image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "RGB to CIE-Lab",      image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Split into RGB",      image_adjustment_group_menu );
    
    image_adjustment_group_menu.separator();

    MENU_ADD_PROCESS_NAME( "Compute Gradients",   image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Watershed Transform 2D",  image_adjustment_group_menu );

  vgui_menu  edge_detection_group_menu; // Edge Detection
    MENU_ADD_PROCESS_NAME( "VD EdgeDetector", edge_detection_group_menu );
    //MENU_ADD_PROCESS_NAME( "Rothwell EdgeDetector", edge_detection_group_menu );
    
    edge_detection_group_menu.separator();

    MENU_ADD_PROCESS_NAME( "Generic Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Third Order Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Compass Edge Detector", edge_detection_group_menu );
    
    edge_detection_group_menu.separator();

    //MENU_ADD_PROCESS_NAME( "RGB Composite Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Generic Color Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Third Order Color Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Compass Color Edge Detector", edge_detection_group_menu );

#if OCTAVE_FOUND
    MENU_ADD_PROCESS_NAME( "Pb Edge Detector (Octave)", edge_detection_group_menu );
#endif

    edge_detection_group_menu.separator();

    MENU_ADD_PROCESS_NAME( "Generic Multiscale Edge Detector", edge_detection_group_menu );
    //MENU_ADD_PROCESS_NAME( "Generic Multicue Edge Detector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Combine Multiple Contour Cues", edge_detection_group_menu );
    
    edge_detection_group_menu.separator();

    MENU_ADD_PROCESS_NAME( "Edge Image to Edge Map", edge_detection_group_menu );

    MENU_ADD_PROCESS_NAME( "Convert Edgemap to Image", edge_detection_group_menu ); // by Nhon, Added by Yuliang 2011/3/7
    
    edge_detection_group_menu.separator();

    MENU_ADD_PROCESS_NAME( "Compute Topographic Curves", edge_detection_group_menu );

    edge_detection_group_menu.separator();
    
    MENU_ADD_PROCESS_NAME( "Generic Edge Linker", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Symbolic Edge Linker", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Hybrid Edge Linker", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Extract Linked Curves", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Curve Fragments", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Curve Fragments Using Logistic Regression", edge_detection_group_menu );
#if OCTAVE_FOUND
    MENU_ADD_PROCESS_NAME( "Kovesi Edge Linker (Octave)", edge_detection_group_menu );
#endif

    MENU_ADD_PROCESS_NAME( "SubSample Edges", edge_detection_group_menu );

    edge_detection_group_menu.separator();
    //MENU_ADD_PROCESS_NAME( "Prune Edgemap", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Edges",  edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Smooth Contours", edge_detection_group_menu);
    
    edge_detection_group_menu.separator();

    MENU_ADD_PROCESS_NAME( "Contour Tracer", edge_detection_group_menu );

    edge_detection_group_menu.separator();

    MENU_ADD_PROCESS_NAME( "Correlate Edgemaps", edge_detection_group_menu );

  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Harris Corners", other_group_menu );

  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
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
    MENU_ADD_TOOL( bvis1_vsol2D_inspect_tool,        vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_adjust_tool,         vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_crop_tool,           vsol2D_tools_menu );
    
  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_keyhole_tool,   soview2D_tools_menu );

  vgui_menu edge_linking_tools_menu;
    MENU_ADD_TOOL( dbdet_crop_edgemap_tool,      edge_linking_tools_menu );
    MENU_ADD_TOOL( dbdet_crop_curvelet_map_tool, edge_linking_tools_menu );

    edge_linking_tools_menu.separator();

    MENU_ADD_TOOL( dbdet_sel_edge_linking_tool,  edge_linking_tools_menu );
    MENU_ADD_TOOL( dbdet_query_linking_tool,     edge_linking_tools_menu );
    MENU_ADD_TOOL( dbdet_sel_explorer_tool,      edge_linking_tools_menu );
    MENU_ADD_TOOL( dbdet_sel_contour_explorer_tool, edge_linking_tools_menu );
    
    edge_linking_tools_menu.separator();

    MENU_ADD_TOOL( dbdet_sel_CC_linking_tool,  edge_linking_tools_menu );
    MENU_ADD_TOOL( dbdet_sel_curvelet_linking_tool, edge_linking_tools_menu );
    MENU_ADD_TOOL( dbdet_sel_DHT_tool, edge_linking_tools_menu );


  tools.add("vsol2D",      vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("Edge Linking", edge_linking_tools_menu);

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

  //////////////// Evaluation Menu ///////////////////// by Yuliang Oct 25 2010
  vgui_menu evaluation_menu;
  MENU_ADD_PROCESS_NAME( "Contour Evaluation", evaluation_menu );
  MainMenu.add( "Evaluation" , evaluation_menu );

  return MainMenu;
}
