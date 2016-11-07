// GUI menu for bae_demo
#include "bae_demo_menu.h"
#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_process_manager.h>

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
#include <dbbgm/vis/dbbgm_inspector_tool.h>


bae_demo_menu::bae_demo_menu()
{
}


vgui_menu
bae_demo_menu::setup_menu()
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
       MENU_ADD_PROCESS_NAME2( "Video", "Save Video", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Image", "Save Image", save_menu);

  file_menu.add("Load..", load_menu);
  file_menu.add("Save..", save_menu);
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
    MENU_ADD_PROCESS_NAME( "Resample",            image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Convert to Grey",     image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Equalize Images",     image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Lens Distort Image",  image_adjustment_group_menu );


  vgui_menu  edge_detection_group_menu; // Edge Detection 
    MENU_ADD_PROCESS_NAME( "VD EdgeDetector",   edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Scale vsol",        edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Lens Distort VSOL", edge_detection_group_menu );


  vgui_menu  bg_model_group_menu; // Background model
    MENU_ADD_PROCESS_NAME( "Init Background", bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Model Background", bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Truth BG Model",   bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Detect Background",   bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "HMM Train Model",  bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "HMM Learn Transitions",  bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "HMM Detect",  bg_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Equalize with Background",  bg_model_group_menu );

  vgui_menu align_3d_model_group_menu; // 3d model alignment
    MENU_ADD_PROCESS_NAME( "Position Model",  align_3d_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Model Classify",  align_3d_model_group_menu );
    MENU_ADD_PROCESS_NAME( "Evaluate Segmentation",  align_3d_model_group_menu );

  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Scan Convert Polygons", other_group_menu );

  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
  processes_menu.add( "Background Model", bg_model_group_menu);
  processes_menu.add( "3D Model Alignment", align_3d_model_group_menu);
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
    MENU_ADD_TOOL( bvis1_vsol2D_adjust_tool,         vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_split_curve_tool,    vsol2D_tools_menu );

  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );

  vgui_menu image_tools_menu;
    MENU_ADD_TOOL( dbbgm_inspector_tool,           image_tools_menu );


  tools.add("vsol2D",      vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("BG Image",       image_tools_menu);

  MainMenu.add( "Tools" , tools );


  /////////////// Tableau Menu ///////////////
  bvis1_tableau_menu  tableau_menu;
  tableau_menu.add_to_menu( MainMenu );


  /////////////// View Menu ////////////////
  bvis1_view_menu view_menu;
  view_menu.add_to_menu(MainMenu);

  return MainMenu;
}


