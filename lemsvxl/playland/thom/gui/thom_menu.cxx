// GUI menu for thom
#include "thom_menu.h"
#include <bvis/bvis_manager.h>
#include <vidpro/vidpro_process_manager.h>
#include <dbgui/dbgui_utils.h>

#include <bvis/menu/bvis_command.h>
#include <bvis/menu/bvis_file_menu.h>
#include <bvis/menu/bvis_video_menu.h>
#include <bvis/menu/bvis_process_macro_menu.h>
#include <bvis/menu/bvis_tableau_menu.h>
#include <bvis/menu/bvis_view_menu.h>
#include <bvis/menu/bvis_repository_menu.h>
#include <bvis/menu/bvis_add_menu.h>

#include <bvis/bvis_tool.h>
#include <bvis/tool/bvis_vsol2D_tools.h>
#include <bvis/tool/bvis_soview2D_tools.h>


thom_menu::thom_menu() 
{
}

static void render_ps()
{
  dbgui_utils::render_to_ps(bvis_manager::instance());
}

vgui_menu
thom_menu::setup_menu()
{
  vgui_menu MainMenu;
  
  /////////////// File Menu ///////////////
  bvis_file_menu file_menu;

  //AMIR:added these to make loading and saving more intuitive
  vgui_menu load_menu;
  vgui_menu save_menu;

  //these have to be customized to each application which is why they cannot
  //be defined in bvis_file_menu

  FILE_MENU_ADD_PROCESS_NAME( "Video", "Load Video", load_menu );
  FILE_MENU_ADD_PROCESS_NAME( "Image", "Load Image", load_menu);

  file_menu.add("Load..", load_menu);
  file_menu.add("Save..", save_menu);
  file_menu.add("Save to PostScript", render_ps);
  file_menu.separator();

  file_menu.add_to_menu( MainMenu );

  
  /////////////// Video Menu ///////////////
  bvis_video_menu  video_menu;
  video_menu.add_to_menu( MainMenu );

  
  /////////////// Repository Menu ////////////////////
  bvis_repository_menu repository_menu;

  repository_menu.add_to_menu( MainMenu );

  
  /////////////// Add Menu ////////////////////
  bvis_add_menu add_menu;

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

  vgui_menu  network_group_menu; // Network
    MENU_ADD_PROCESS_NAME( "Build MRF Network", network_group_menu );

  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Harris Corners", other_group_menu );

  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Motion Detection", motion_detection_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
  processes_menu.add( "Network",          network_group_menu);
  processes_menu.add( "Other",            other_group_menu);

  MainMenu.add( "Processes" , processes_menu );


  /////////////// Process Macro Menu ///////////////

  bvis_process_macro_menu  process_macro_menu;
  process_macro_menu.add_to_menu (MainMenu);


  /////////////// Tools Menu ///////////////
  vgui_menu tools;
  vgui_menu vsol2D_tools_menu, vsol2D_draw_tools_menu;
    MENU_ADD_TOOL( bvis_vsol2D_line_tool,           vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis_vsol2D_point_tool,          vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis_vsol2D_polyline_tool,       vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis_vsol2D_polygon_tool,        vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis_vsol2D_digital_curve_tool,  vsol2D_draw_tools_menu );
    vsol2D_tools_menu.add("Drawing", vsol2D_draw_tools_menu);
    MENU_ADD_TOOL( bvis_vsol2D_split_curve_tool,    vsol2D_tools_menu );
  
  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis_style_tool,     soview2D_tools_menu );
    
  tools.add("vsol2D", vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);

  MainMenu.add( "Tools" , tools );


  /////////////// Tableau Menu ///////////////
  bvis_tableau_menu  tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  
  /////////////// View Menu ////////////////
  bvis_view_menu view_menu;
  view_menu.add_to_menu(MainMenu);

  return MainMenu;
}


