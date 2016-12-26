//This is contrib/ozge/gui/matching_gui_menu.cxx

#include "face_gui_menu.h"

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

#include <ozge/face_gui/vis/dbru_facedb_tools.h>

matching_gui_menu::matching_gui_menu()
{
}


vgui_menu
matching_gui_menu::setup_menu()
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
  //FILE_MENU_ADD_PROCESS_NAME( "Video and Polys", "Load Video And Polys", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Image", "Load Image", load_menu);
  //FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Load .bnd File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( ".CON", "Load .CON File", load_menu);
  //FILE_MENU_ADD_PROCESS_NAME( ".EDG", "Load .EDG File", load_menu );
  //FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Load .CEM File", load_menu );
  //FILE_MENU_ADD_PROCESS_NAME( ".ESF", "Load .ESF File", load_menu );
 
  //FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Save .bnd File", save_menu);
  //FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Save .CEM File", save_menu);
  MENU_ADD_PROCESS_NAME2( ".CON", "Save .CON File", save_menu);
  //FILE_MENU_ADD_PROCESS_NAME( ".ESF", "Save .ESF File", save_menu);
  

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

  vgui_menu  edge_detection_group_menu; // Edge Detection
  MENU_ADD_PROCESS_NAME( "VD EdgeDetector", edge_detection_group_menu );
  //MENU_ADD_PROCESS_NAME( "Logical Linear",  edge_detection_group_menu );
  //MENU_ADD_PROCESS_NAME( "Susan",           edge_detection_group_menu );
  MENU_ADD_PROCESS_NAME( "Contour Tracer", edge_detection_group_menu );
  //MENU_ADD_PROCESS_NAME( "Prune Edges",  edge_detection_group_menu );
  MENU_ADD_PROCESS_NAME( "Smooth Edges", edge_detection_group_menu);

  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Harris Corners", other_group_menu );
//    MENU_ADD_PROCESS_NAME( "Show TPS", other_group_menu );

 
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
  processes_menu.add( "Other",            other_group_menu);
 
  MainMenu.add( "Processes" , processes_menu );

  vgui_menu  facedb_menu; // facedb 
  //MENU_ADD_PROCESS_NAME( "Run query on facedb", facedb_menu );
  MENU_ADD_PROCESS_NAME( "Create Empty facedb", facedb_menu );  
  MENU_ADD_PROCESS_NAME( "Load facedb", facedb_menu );
  MENU_ADD_PROCESS_NAME( "Save facedb", facedb_menu );
  //MENU_ADD_PROCESS_NAME( "Save facedb for dll", facedb_menu);

  vgui_menu facedb_tools_menu;
  //MENU_ADD_TOOL( dbru_facedb_delete_observations_tool, facedb_tools_menu);
  MENU_ADD_TOOL( dbru_facedb_add_subjects_tool, facedb_tools_menu );
  //MENU_ADD_TOOL( dbru_facedb_match_tool, facedb_tools_menu);
  //MENU_ADD_TOOL( dbru_facedb_save_db_file_tool, facedb_tools_menu);
    
  facedb_menu.add("facedb Tools", facedb_tools_menu);
  MainMenu.add( "FACE DB" , facedb_menu );
  
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

  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );   

  tools.add("vsol2D",      vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);

  MainMenu.add( "Tools", tools );

  /////////////// Tableau Menu ///////////////
  bvis1_tableau_menu tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  /////////////// View Menu ////////////////
  bvis1_view_menu view_menu;
  view_menu.add_to_menu(MainMenu);

  return MainMenu;
}
