// This is brcv/seg/dbdet/vis/dbdet_dlvwr_tool.cxx
//:
// \file

// GUI menu for medial_livewire
#include "medial_livewire_menu.h"
#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <dbgui/dbgui_utils.h>

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
#include <bvis1/tool/bvis1_correspond_tools.h>
#include <dbdet/vis/dbdet_livewire_tool.h>
#include <dbdet/vis/dbdet_medial_livewire_tool.h>
#include "dbdet_draw_spheres_tool.h"
#include "dbdet_dlvwr_tool.h"
#include "dbdet_image_seg_dlvwr_tool.h"
#include "dbdet_dmedial_wire_tool.h"

#include <dbsol/vis/dbsol_cut_contour_tool.h>
#include "dbdet_manual_seg_tool.h"


medial_livewire_menu::medial_livewire_menu() 
{
}

static void render_ps()
{
  dbgui_utils::render_to_ps(bvis1_manager::instance());
}

vgui_menu
medial_livewire_menu::setup_menu()
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
  FILE_MENU_ADD_PROCESS_NAME(".CON", "Load .CON File", load_menu);
  FILE_MENU_ADD_PROCESS_NAME("volume seg xml", "load volume seg. xml", load_menu);

  FILE_MENU_ADD_PROCESS_NAME("save seg3d_info .xml", "save seg3d_info .xml", save_menu);
  FILE_MENU_ADD_PROCESS_NAME("new seg3d_info .xml", "new seg3d_info .xml", file_menu);

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


  /////////////// Processes Menu ///////////////
  vgui_menu processes_menu;

  vgui_menu  image_adjustment_group_menu; // Image Adjustment
    MENU_ADD_PROCESS_NAME( "Brightness/Contrast", image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Gaussian Blur",       image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Convert to Grey",     image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME("ROI Image",           image_adjustment_group_menu );

  vgui_menu  motion_detection_group_menu; // Motion Detection
    MENU_ADD_PROCESS_NAME( "Frame Difference", motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Motion",   motion_detection_group_menu );

  vgui_menu  edge_detection_group_menu; // Edge Detection
    MENU_ADD_PROCESS_NAME( "VD EdgeDetector", edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Contour Tracer", edge_detection_group_menu );

  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Harris Corners", other_group_menu );
    MENU_ADD_PROCESS_NAME( "isosurface of seg3d_info", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Isosurface", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Smooth Contours", other_group_menu );
    MENU_ADD_PROCESS_NAME( "Merge vsol2D storages", other_group_menu );
    

  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Motion Detection", motion_detection_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
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
    MENU_ADD_TOOL( bvis1_vsol2D_split_curve_tool,    vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_correspond_point_tool,      vsol2D_tools_menu );
    MENU_ADD_TOOL( dbsol_cut_contour_tool,          vsol2D_tools_menu );
  
  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );
    
  tools.add("vsol2D", vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  MENU_ADD_TOOL( dbdet_livewire_tool, tools );
  MENU_ADD_TOOL( dbdet_medial_livewire_tool, tools );
  MENU_ADD_TOOL( dbdet_draw_spheres_tool, tools );
  MENU_ADD_TOOL( dbdet_dlvwr_tool, tools );
  MENU_ADD_TOOL( dbdet_manual_seg_tool, tools );
  MENU_ADD_TOOL( dbdet_image_seg_dlvwr_tool, tools );
  MENU_ADD_TOOL( dbdet_dmedial_wire_tool, tools );

  MainMenu.add( "Tools" , tools );


  /////////////// Tableau Menu ///////////////
  bvis1_tableau_menu  tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  
  /////////////// View Menu ////////////////
  bvis1_view_menu view_menu;
  view_menu.add_to_menu(MainMenu);

  return MainMenu;
}


