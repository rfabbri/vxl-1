// GUI menu for itrack2
#include "itrack2_menu.h"
#include <bvis/bvis_video_manager.h>
#include <vidpro/vidpro_process_manager.h>
#include <dbgui/dbgui_utils.h>

#include <bvis/menu/bvis_command.h>
#include <bvis/menu/bvis_file_menu.h>
#include <bvis/menu/video/bvis_video_file_menu.h>
#include <bvis/menu/video/bvis_video_menu.h>
#include <bvis/menu/bvis_process_macro_menu.h>
#include <bvis/menu/bvis_tableau_menu.h>
#include <bvis/menu/bvis_view_menu.h>
#include <bvis/menu/video/bvis_video_view_menu.h>
#include <bvis/menu/bvis_repository_menu.h>
#include <bvis/menu/video/bvis_video_add_menu.h>

#include <bvis/bvis_tool.h>
#include <bvis/tool/bvis_vsol2D_tools.h>
#include <bvis/tool/bvis_soview2D_tools.h>
#include <bvis/tool/bvis_correspond_tools.h>
#include <dbinfo/vis/dbinfo_track_tools.h>

itrack2_menu::itrack2_menu() 
{
}

static void render_ps()
{
  dbgui_utils::render_to_ps(bvis_video_manager::instance());
}

vgui_menu
itrack2_menu::setup_menu()
{
  vgui_menu MainMenu;
  
  /////////////// File Menu ///////////////
  bvis_file_menu<bvis_video_manager> file_menu;
  bvis_video_file_menu video_file_menu;
  //AMIR:added these to make loading and saving more intuitive
  vgui_menu load_menu;
  vgui_menu save_menu;
  vgui_menu open_menu;

  //these have to be customized to each application which is why they cannot
  //be defined in bvis_file_menu

 FILE_MENU_ADD_PROCESS_NAME( "Video Input Stream", "Open Video Istream", open_menu, bvis_video_manager);
 FILE_MENU_ADD_PROCESS_NAME( "Video Output Stream", "Open Video Ostream", open_menu, bvis_video_manager);
  FILE_MENU_ADD_PROCESS_NAME( "Video", "Load Video", load_menu, bvis_video_manager);
  FILE_MENU_ADD_PROCESS_NAME( "Image", "Load Image", load_menu, bvis_video_manager );

  //Can't use macro in this case since I want to allow the queue
  ///==================== expanded macro =================================
  save_menu.add("Video",
                new bvis_process_command<bvis_video_manager>(bvis_video_manager::instance()->
                                         process_manager()->
                                         get_process_by_name("Save Video"),
                                         true));
  //                  allow queue ---------^
  //======================================================================

  FILE_MENU_ADD_PROCESS_NAME( "Tracks","Load Tracks", load_menu, bvis_video_manager);
  FILE_MENU_ADD_PROCESS_NAME( "Tracks","Save Tracks", save_menu, bvis_video_manager);

  FILE_MENU_ADD_PROCESS_NAME( "VJ Polys","Load VJ_Polys", load_menu, bvis_video_manager);

  file_menu.add("Open..", open_menu);
  file_menu.add("Load..", load_menu);
  file_menu.add("Save..", save_menu);
  video_file_menu.add_to_menu(file_menu);
  file_menu.add("Save to PostScript", render_ps);
  file_menu.separator();

  file_menu.add_to_menu( MainMenu );

  
  /////////////// Video Menu ///////////////
  bvis_video_menu  video_menu;
  video_menu.add_to_menu( MainMenu );

  
  /////////////// Repository Menu ////////////////////
  bvis_repository_menu<bvis_video_manager> repository_menu;

  repository_menu.add_to_menu( MainMenu );

  
  /////////////// Add Menu ////////////////////
  bvis_video_add_menu add_menu;

  add_menu.add_to_menu( MainMenu );


  /////////////// Processes Menu ///////////////
  vgui_menu processes_menu;

  vgui_menu  image_adjustment_group_menu; // Image Adjustment
  MENU_ADD_PROCESS_NAME( "Brightness/Contrast", image_adjustment_group_menu, bvis_video_manager );
  MENU_ADD_PROCESS_NAME( "Gaussian Blur",       image_adjustment_group_menu, bvis_video_manager );
  MENU_ADD_PROCESS_NAME( "Convert to Grey",     image_adjustment_group_menu, bvis_video_manager );

  vgui_menu  frame_processing_menu; // Frame Processing
  MENU_ADD_PROCESS_NAME( "Frame Difference", frame_processing_menu, bvis_video_manager );
  MENU_ADD_PROCESS_NAME( "Compute Motion",   frame_processing_menu, bvis_video_manager );

  vgui_menu detection_menu; // Detection
  MENU_ADD_PROCESS_NAME( "VD EdgeDetector", detection_menu, bvis_video_manager );
  MENU_ADD_PROCESS_NAME( "Harris Corners", detection_menu, bvis_video_manager );
  MENU_ADD_PROCESS_NAME( "Crazy Global Harris Corners", detection_menu, bvis_video_manager );
  MENU_ADD_PROCESS_NAME( "Aerial BG model builder", detection_menu, bvis_video_manager );
  MENU_ADD_PROCESS_NAME( "Aerial Foreground Detection Uncertainity",
                         detection_menu, bvis_video_manager );
  MENU_ADD_PROCESS_NAME( "Blob Finder", detection_menu, bvis_video_manager );

  vgui_menu  tracking_menu; // Object Tracking
  MENU_ADD_PROCESS_NAME( "dbinfo_multi_track_process", tracking_menu, bvis_video_manager );
  MENU_ADD_PROCESS_NAME( "Refine Tracks", tracking_menu, bvis_video_manager );

  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Frame Processing", frame_processing_menu );
  processes_menu.add( "Detection",   detection_menu);
  processes_menu.add( "Tracking",    tracking_menu);

  MainMenu.add( "Processes" , processes_menu );


  /////////////// Process Macro Menu ///////////////

  bvis_process_macro_menu<bvis_video_manager>  process_macro_menu;
  process_macro_menu.add_to_menu (MainMenu);


  /////////////// Tools Menu ///////////////
  vgui_menu tools;
  vgui_menu vsol2D_tools_menu, vsol2D_draw_tools_menu,soview2D_tools_menu;

  MENU_ADD_TOOL( bvis_vsol2D_line_tool,           vsol2D_draw_tools_menu , bvis_video_manager);
  MENU_ADD_TOOL( bvis_vsol2D_point_tool,          vsol2D_draw_tools_menu , bvis_video_manager);
  MENU_ADD_TOOL_USER_IO( bvis_vsol2D_polyline_tool, vsol2D_draw_tools_menu , bvis_video_manager);
  //  MENU_ADD_TOOL( bvis_vsol2D_polyline_tool,       vsol2D_draw_tools_menu );

  MENU_ADD_TOOL( bvis_vsol2D_polygon_tool,        vsol2D_draw_tools_menu, bvis_video_manager );

  MENU_ADD_TOOL( bvis_vsol2D_digital_curve_tool,  vsol2D_draw_tools_menu, bvis_video_manager );

  vsol2D_tools_menu.add("Drawing", vsol2D_draw_tools_menu);
#if 0
  MENU_ADD_TOOL( bvis_vsol2D_split_curve_tool,    vsol2D_tools_menu );
  MENU_ADD_TOOL( bvis_correspond_point_tool,      vsol2D_tools_menu );
  
  vgui_menu soview2D_tools_menu;
  MENU_ADD_TOOL( bvis_translate_tool, soview2D_tools_menu );
  MENU_ADD_TOOL( bvis_style_tool,     soview2D_tools_menu );
#endif
  vgui_menu dbinfo_tools_menu;
  ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbinfo_track_describe_tool, dbinfo_tools_menu );
  ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbinfo_track_edit_tool, dbinfo_tools_menu );
  ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbinfo_region_minfo_tool, dbinfo_tools_menu );
  tools.add("vsol2D", vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("itrack2", dbinfo_tools_menu);

  MainMenu.add( "Tools" , tools );


  /////////////// Tableau Menu ///////////////
  bvis_tableau_menu<bvis_video_manager>  tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  
  /////////////// View Menu ////////////////
  bvis_view_menu<bvis_video_manager> view_menu;
  bvis_video_view_menu video_view_menu;
  video_view_menu.add_to_menu(view_menu);
  view_menu.add_to_menu(MainMenu);
  
  return MainMenu;
}


