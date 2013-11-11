// GUI menu for imatch
#include "imatch_menu.h"
#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <dbgui/dbgui_utils.h>

#include <bvis1/menu/bvis1_command.h>
#include <bvis1/menu/bvis1_file_menu.h>
#include <bvis1/menu/bvis1_video_menu.h>
#include <bvis1/menu/bvis1_tableau_menu.h>
#include <bvis1/menu/bvis1_view_menu.h>
#include <bvis1/menu/bvis1_repository_menu.h>
#include <bvis1/menu/bvis1_add_menu.h>

#include <bvis1/bvis1_tool.h>
#include <bvis1/tool/bvis1_vsol2D_tools.h>
#include <bvis1/tool/bvis1_soview2D_tools.h>
#include <bvis1/tool/bvis1_correspond_tools.h>
#include <dbinfo/vis/dbinfo_osl_tools.h>

imatch_menu::imatch_menu() 
{
}

static void render_ps()
{
  dbgui_utils::render_to_ps(bvis1_manager::instance());
}

vgui_menu
imatch_menu::setup_menu()
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
  FILE_MENU_ADD_PROCESS_NAME( "OSL", "Load OSL", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( "OSL", "Save OSL", save_menu);

  //Can't use macro in this case since I want to allow the queue
  ///==================== expanded macro =================================
  save_menu.add("Video",
                new bvis1_process_command(bvis1_manager::instance()->
                                         process_manager()->
                                         get_process_by_name("Save Video"),
                                         true));
  //                  allow queue ---------^
  //======================================================================

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


  /////////////// Matching Menu ///////////////

  vgui_menu  matching_menu; // Object Matching
  MENU_ADD_PROCESS_NAME( "Minfo Match Images", matching_menu );

  MainMenu.add( "Matching" ,matching_menu );

  
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
  MENU_ADD_TOOL( bvis1_correspond_point_tool,      vsol2D_tools_menu );
  
  vgui_menu soview2D_tools_menu;
  MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
  MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );
    
  vgui_menu dbinfo_tools_menu;
  MENU_ADD_TOOL( dbinfo_osl_edit_tool, dbinfo_tools_menu );
  MENU_ADD_TOOL( dbinfo_osl_match_tool, dbinfo_tools_menu );
  MENU_ADD_TOOL( dbinfo_osl_transform_tool, dbinfo_tools_menu );

  tools.add("vsol2D", vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("dbinfo", dbinfo_tools_menu);

  MainMenu.add( "Tools" , tools );


  /////////////// Tableau Menu ///////////////
  bvis1_tableau_menu  tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  
  /////////////// View Menu ////////////////
  bvis1_view_menu view_menu;
  view_menu.add_to_menu(MainMenu);

  return MainMenu;
}


