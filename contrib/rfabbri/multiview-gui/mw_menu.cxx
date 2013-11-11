#include "mw_menu.h"
#include "geno_app.h"
#include "mw_app.h"
#include "epip_app.h"

#include <bvis1/bvis1_manager.h>
#include <bvis1/menu/bvis1_command.h>

#include <dbgui/dbgui_utils.h>


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
#include <dbdet/vis/dbdet_livewire_tool.h>


#include <vcl_iostream.h>

mw_menu::mw_menu()
{
}


vgui_menu
mw_menu::setup_menu()
{

  vgui_menu MainMenu;


  /////////////// File Menu ///////////////
  bvis1_file_menu file_menu;

  //AMIR:added these to make loading and saving more intuitive
  vgui_menu load_menu;
  vgui_menu save_menu;

  //these have to be customized to each application which is why they cannot
  //be defined in bvis1_file_menu

//  FILE_MENU_ADD#include <vidpro1/vidpro1_repository.h>_PROCESS_NAME( "Video", "Load Video", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( "Image", "Load Image", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Load .bnd File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CON", "Load .CON File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".EDG", "Load .EDG File", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Load .CEM File", load_menu );

  FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Save .bnd File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Save .CEM File", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CON", "Save .CON File", save_menu);

  file_menu.add("Load..", load_menu);
  file_menu.add("Save..", save_menu);
  file_menu.separator();

  file_menu.add_to_menu( MainMenu );

  /////////////// Video Mrecebi seu emailenu ///////////////
//  bvis1_video_menu video_menu;
//  video_menu.add_to_menu( MainMenu );

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

//  vgui_menu  motion_detection_group_menu; // Motion Detection
//    MENU_ADD_PROCESS_NAME( "Frame Difference", motion_detection_group_menu );
//    MENU_ADD_PROCESS_NAME( "Background Difference", motion_detection_group_menu );
//    MENU_ADD_PROCESS_NAME( "Compute Motion", motion_detection_group_menu );
//    MENU_ADD_PROCESS_NAME( "Register Video", motion_detection_group_menu );

  vgui_menu  edge_detection_group_menu; // Edge Detection
    MENU_ADD_PROCESS_NAME( "VD EdgeDetector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Edges",  edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Smooth Edges", edge_detection_group_menu);


  vgui_menu  curve_group_menu; // Curve
    MENU_ADD_PROCESS_NAME( "Curve Distance Map", curve_group_menu );
    MENU_ADD_PROCESS_NAME( "Curve Matching", curve_group_menu );
    MENU_ADD_PROCESS_NAME( "Interpolated Curve Matching", curve_group_menu );
    MENU_ADD_PROCESS_NAME( "Sample Curve", curve_group_menu );
    MENU_ADD_PROCESS_NAME( "Recorded Wire", curve_group_menu );

  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Sample Function", other_group_menu );

//    MENU_ADD_PROCESS_NAME( "Harris Corners", other_group_menu );
 //   MENU_ADD_PROCESS_NAME( "KL Tracking", other_group_menu );
//    MENU_ADD_PROCESS_NAME( "KL Track and Translate", other_group_menu );

  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
//  processes_menu.add( "Motion Detection", motion_detection_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
  processes_menu.add( "Curve",            curve_group_menu );
//  processes_menu.add( "tracking",         ctrk_group_menu);
  processes_menu.add( "Other",            other_group_menu);
 

  MainMenu.add( "Processes" , processes_menu );
  

  /////////////// Process Macro Menu ///////////////
  bvis1_process_macro_menu process_macro_menu;
  process_macro_menu.add_to_menu (MainMenu);

  /////////////// Tools Menu ///////////////
  vgui_menu tools;
   
//  vgui_menu image_tools_menu;
//  MENU_ADD_TOOL(dbvis1_image_intensity_inspector, image_tools_menu );

  vgui_menu vsol2D_tools_menu, vsol2D_draw_tools_menu;
    MENU_ADD_TOOL( bvis1_vsol2D_line_tool,           vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_point_tool,          vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_polyline_tool,       vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_polygon_tool,        vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_digital_curve_tool,  vsol2D_draw_tools_menu );
    vsol2D_tools_menu.add("Drawing", vsol2D_draw_tools_menu);
    MENU_ADD_TOOL( bvis1_vsol2D_split_curve_tool,    vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_correspond_point_tool,      vsol2D_tools_menu );

  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );

    vgui_menu lvwr_tools_menu;
    MENU_ADD_TOOL( dbdet_livewire_tool,              lvwr_tools_menu );


  tools.add("vsol2D",      vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
//  tools.add("Curve Tracking", dbctrk_tools_menu);
  tools.add("Livewire", lvwr_tools_menu);


  MainMenu.add( "Tools", tools );

  /////////////// Tableau Menu ///////////////
  bvis1_tableau_menu tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  /////////////// View Menu ////////////////
  bvis1_view_menu view_menu;
  view_menu.add_to_menu(MainMenu);
  
  ////////// Apps menu ////////////

  vgui_menu apps_menu;

  apps_menu.add("GENO Init",geno_interactive);
  apps_menu.add("GENO Restart",geno_reset);
  apps_menu.separator();
  MENU_ADD_PROCESS_NAME("Show Contours", apps_menu);
  apps_menu.separator();
  apps_menu.add("Epipolar Inspector",epip_interactive);
  apps_menu.separator();
  apps_menu.add("Random",mw_misc);

  MainMenu.add("Apps",apps_menu);

  return MainMenu;
}
