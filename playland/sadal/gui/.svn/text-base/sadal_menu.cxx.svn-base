// GUI menu for sadal
#include "sadal_menu.h"
#include <bvis/bvis_manager.h>
#include <vidpro/vidpro_process_manager.h>

#include <bvis/menu/bvis_command.h>
#include <bvis/menu/bvis_file_menu.h>
#include <bvis/menu/bvis_video_menu.h>
#include <bvis/menu/bvis_repository_menu.h>
#include <bvis/menu/bvis_process_macro_menu.h>
#include <bvis/menu/bvis_tableau_menu.h>
#include <bvis/menu/bvis_view_menu.h>
#include <bvis/bvis_tool.h>
#include <bvis/tool/bvis_vsol2D_tools.h>
#include <bvis/tool/bvis_soview2D_tools.h>
#include <dbvis/tool/dbvis_homog_view_tools.h>

#include <dbvis/tool/dbvis_polyg_select_tool.h>
#if HAS_BGUI3D
#include <dbvis/tool/dbvis_so3d_info_tool.h>
#endif
sadal_menu::sadal_menu() 
{
}


vgui_menu
sadal_menu::setup_menu()
{
  vgui_menu MainMenu;
  

  /////////////// File Menu ///////////////
  bvis_file_menu file_menu;
  file_menu.add_to_menu( MainMenu );

  bvis_view_menu view_menu;
  view_menu.add_to_menu( MainMenu);

  /////////////// Video Menu ///////////////
  bvis_video_menu  video_menu;
  video_menu.add_to_menu( MainMenu );

  /////////////Repostitory Menu/////////////
  bvis_repository_menu repository_menu;
  repository_menu.add_to_menu( MainMenu );

  /////////////// Processes Menu ///////////////
  vgui_menu processes_menu;
        
  vgui_menu file_io_group_menu;  // File I/O
    MENU_ADD_PROCESS_NAME( "Load Image",  file_io_group_menu);
    MENU_ADD_PROCESS_NAME( "Load Video", file_io_group_menu);

    vgui_menu  image_adjustment_group_menu; // Image Adjustment
    MENU_ADD_PROCESS_NAME( "Brightness/Contrast", image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Gaussian Blur",       image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Convert to Grey",     image_adjustment_group_menu );

  vgui_menu  motion_detection_group_menu; // Motion Detection
    MENU_ADD_PROCESS_NAME( "Frame Difference", motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Motion",   motion_detection_group_menu );
        MENU_ADD_PROCESS_NAME( "Kanade-Lucas Tracking" , motion_detection_group_menu );
        MENU_ADD_PROCESS_NAME( "Harris Corners" , motion_detection_group_menu );

  vgui_menu  edge_detection_group_menu; // Edge Detection
    MENU_ADD_PROCESS_NAME( "VD EdgeDetector", edge_detection_group_menu );

  
  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Reconstruct by Factorization" , other_group_menu );
    #if HAS_BGUI3D
    MENU_ADD_PROCESS_NAME( "Affine Transform Estimation" , other_group_menu );
#endif
    MENU_ADD_PROCESS_NAME( "Calc Plane Parameters", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Drawing Tablo",other_group_menu );
    MENU_ADD_PROCESS_NAME( "Homography Test",other_group_menu );
        MENU_ADD_PROCESS_NAME( "Combine Storage",other_group_menu );
        

  processes_menu.add( "File I/O",         file_io_group_menu );
  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Motion Detection", motion_detection_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
 
  processes_menu.add( "My Processes",            other_group_menu);
  

  MainMenu.add( "Processes" , processes_menu );


  /////////////// Process Macro Menu ///////////////

  bvis_process_macro_menu  process_macro_menu;
  process_macro_menu.add_to_menu (MainMenu);


  /////////////// Tools Menu ///////////////
  vgui_menu tools;
  vgui_menu vsol2D_tools_menu;
    MENU_ADD_TOOL( bvis_vsol2D_line_tool,     vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis_vsol2D_point_tool,    vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis_vsol2D_polyline_tool, vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis_vsol2D_polygon_tool,  vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis_vsol2D_inspect_tool,  vsol2D_tools_menu );
    MENU_ADD_TOOL( dbvis_polyg_select_tool,   vsol2D_tools_menu );
        
        
  
  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis_translate_tool, soview2D_tools_menu );

  vgui_menu homog_view_tools_menu;
   MENU_ADD_TOOL( dbvis_homog_view_tool,  homog_view_tools_menu );
#if HAS_BGUI3D
   vgui_menu so3d_tools_menu;
   MENU_ADD_TOOL( dbvis_so3d_info_tool, so3d_tools_menu );
#endif        
        
    
  tools.add("Draw vsol2D", vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("Homography View", homog_view_tools_menu);
#if HAS_BGUI3D
  tools.add("3D tools", so3d_tools_menu);
#endif
  

  MainMenu.add( "Tools" , tools );


  /////////////// Tableau Menu ///////////////
  bvis_tableau_menu  tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  return MainMenu;
}


