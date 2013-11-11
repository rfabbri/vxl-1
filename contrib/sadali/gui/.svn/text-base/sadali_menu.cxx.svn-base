// GUI menu for sadali
#include "sadali_menu.h"
#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_process_manager.h>

#include <bvis1/menu/bvis1_command.h>
#include <bvis1/menu/bvis1_file_menu.h>
#include <bvis1/menu/bvis1_video_menu.h>
#include <bvis1/menu/bvis1_repository_menu.h>
#include <bvis1/menu/bvis1_process_macro_menu.h>
#include <bvis1/menu/bvis1_tableau_menu.h>
#include <bvis1/menu/bvis1_view_menu.h>
#include <bvis1/bvis1_tool.h>
#include <bvis1/tool/bvis1_vsol2D_tools.h>
#include <bvis1/tool/bvis1_soview2D_tools.h>
#include <dbhom_vis/dbvis1_homog_view_tools.h>

#include <dbbbopt_vis/dbvis1_polyg_select_img_tool.h>
#include <dbbbopt_vis/dbvis1_polyg_select_tool.h>

sadali_menu::sadali_menu() 
{
}


vgui_menu
sadali_menu::setup_menu()
{
  vgui_menu MainMenu;
  

  /////////////// File Menu ///////////////
  bvis1_file_menu file_menu;
  file_menu.add_to_menu( MainMenu );

  bvis1_view_menu view_menu;
  view_menu.add_to_menu( MainMenu);

  /////////////// Video Menu ///////////////
  bvis1_video_menu  video_menu;
  video_menu.add_to_menu( MainMenu );

  /////////////Repostitory Menu/////////////
  bvis1_repository_menu repository_menu;
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
    MENU_ADD_PROCESS_NAME( "Edge2Img", edge_detection_group_menu );
     MENU_ADD_PROCESS_NAME( "DigCur2Img", edge_detection_group_menu );
    
    vgui_menu BoundingBox_refinement_menu;
    MENU_ADD_PROCESS_NAME( "BB Plane Optimization",BoundingBox_refinement_menu );
    MENU_ADD_PROCESS_NAME( "BB 2D Projection", BoundingBox_refinement_menu );
    MENU_ADD_PROCESS_NAME( "3D Planar Curve Reconstruction", BoundingBox_refinement_menu );
    MENU_ADD_PROCESS_NAME( "Curve Homography", BoundingBox_refinement_menu);



  vgui_menu  multiview_geom_menu; // Other
    MENU_ADD_PROCESS_NAME( "Reconstruct by Factorization" , multiview_geom_menu );
    #if HAS_BGUI3D
    MENU_ADD_PROCESS_NAME( "Affine Transform Estimation" , multiview_geom_menu );
#endif
    MENU_ADD_PROCESS_NAME( "Calc Plane Parameters", multiview_geom_menu );
    MENU_ADD_PROCESS_NAME( "Drawing Tablo",multiview_geom_menu );
    MENU_ADD_PROCESS_NAME( "Homography Test",multiview_geom_menu);
    MENU_ADD_PROCESS_NAME( "Homography Test_Img",multiview_geom_menu);
    MENU_ADD_PROCESS_NAME( "Combine Storage",multiview_geom_menu );
        

  processes_menu.add( "File I/O",         file_io_group_menu );
  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Motion Detection", motion_detection_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
   processes_menu.add( "Bounding Box Refinement",   BoundingBox_refinement_menu );
 
  processes_menu.add( "Multiview Geometry Processes", multiview_geom_menu);
  

  MainMenu.add( "Processes" , processes_menu );


  /////////////// Process Macro Menu ///////////////

  bvis1_process_macro_menu  process_macro_menu;
  process_macro_menu.add_to_menu (MainMenu);


  /////////////// Tools Menu ///////////////
  vgui_menu tools;
  vgui_menu vsol2D_tools_menu;
    MENU_ADD_TOOL( bvis1_vsol2D_line_tool,     vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_point_tool,    vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_polyline_tool, vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_polygon_tool,  vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_inspect_tool,  vsol2D_tools_menu );
    MENU_ADD_TOOL( dbvis1_polyg_select_tool,   vsol2D_tools_menu );
        
        
  
  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );

  vgui_menu homog_view_tools_menu;
   MENU_ADD_TOOL( dbvis1_homog_view_tool,  homog_view_tools_menu );

   vgui_menu BoundingBox_refine_menu;
   MENU_ADD_TOOL( dbvis1_polyg_select_img_tool,  BoundingBox_refine_menu );

#if HAS_BGUI3D
   vgui_menu so3d_tools_menu;
   MENU_ADD_TOOL( dbvis1_so3d_info_tool, so3d_tools_menu );
#endif        
        
    
  tools.add("Draw vsol2D", vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("Homography View", homog_view_tools_menu);
#if HAS_BGUI3D
  tools.add("3D tools", so3d_tools_menu);
#endif
  tools.add("Image_editing", BoundingBox_refine_menu);

  MainMenu.add( "Tools" , tools );


  /////////////// Tableau Menu ///////////////
  bvis1_tableau_menu  tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  return MainMenu;
}


