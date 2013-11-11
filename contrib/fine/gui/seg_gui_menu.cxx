//This is contrib/seg/gui/seg_gui_menu.cxx

#include "seg_gui_menu.h"

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

#include <vis/dbseg_seg_create_region_tool.h>
#include <vis/dbseg_seg_edit_region_tool.h>
#include <vis/dbseg_seg_label_region_tool.h>
#include <vis/dbseg_seed_regions_tool.h>

seg_gui_menu::seg_gui_menu()
{
}

vgui_menu
seg_gui_menu::setup_menu()
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
  FILE_MENU_ADD_PROCESS_NAME( "Image Sequence", "Load Image Sequence", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Segmentation Structure", "Load Segmentation Structure", load_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Segmentation Sequence", "Load Segmentation Sequence", load_menu);

  FILE_MENU_ADD_PROCESS_NAME( "Image", "Save Image", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Segmentation Structure", "Save Segmentation Structure", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Segmentation View", "Save Segmentation View", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Segmentation Contour", "Save Segmentation Contour", save_menu);
  FILE_MENU_ADD_PROCESS_NAME( "Binary Image", "Save Segmentation Binary Image", save_menu);

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

  //////////////// Segmentation Menu ////////////////
  vgui_menu segmentation_menu;
        
     MENU_ADD_PROCESS_NAME( "Create Seg Structure", segmentation_menu );
     MENU_ADD_PROCESS_NAME( "Isosurface", segmentation_menu );
     MENU_ADD_PROCESS_NAME( "Disk Measurements", segmentation_menu );
     MENU_ADD_PROCESS_NAME( "Disk Binary Images", segmentation_menu );

  MainMenu.add("Segmentation", segmentation_menu);

  /////////////// Processes Menu ///////////////
  vgui_menu processes_menu;

  
  vgui_menu  image_adjustment_group_menu; // Image Adjustment
    /*MENU_ADD_PROCESS_NAME( "Brightness/Contrast", image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Gaussian Blur",       image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Convert to Grey",     image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "RGB to IHS",          image_adjustment_group_menu );*/
    MENU_ADD_PROCESS_NAME( "Watershed Transform 2D",  image_adjustment_group_menu );

    MENU_ADD_PROCESS_NAME( "Mean Shift Segmentation",  image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "Graph Segmentation",  image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "JSEG Segmentation",  image_adjustment_group_menu );
    //MENU_ADD_PROCESS_NAME( "Subpixel Segmentation",  image_adjustment_group_menu );
    MENU_ADD_PROCESS_NAME( "SRGtek Segmentation",  image_adjustment_group_menu );
    //MENU_ADD_PROCESS_NAME( "Level Set Segmentation",  image_adjustment_group_menu );

    MENU_ADD_PROCESS_NAME( "Geodesic Active Contour",  image_adjustment_group_menu );

  vgui_menu  motion_detection_group_menu; // Motion Detection
    /*MENU_ADD_PROCESS_NAME( "Frame Difference", motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Background Difference", motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Compute Motion", motion_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Register Video", motion_detection_group_menu );*/


  vgui_menu  other_group_menu; // Other
    //MENU_ADD_PROCESS_NAME( "Harris Corners", other_group_menu );


  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Motion Detection", motion_detection_group_menu );
  processes_menu.add( "Other",            other_group_menu);
 

  MainMenu.add( "Processes" , processes_menu );
  

  /////////////// Process Macro Menu ///////////////
  bvis1_process_macro_menu process_macro_menu;
  process_macro_menu.add_to_menu (MainMenu);

  /////////////// Tools Menu ///////////////
  vgui_menu tools;
   MENU_ADD_TOOL( dbseg_seg_create_region_tool,        tools);
   MENU_ADD_TOOL( dbseg_seg_edit_region_tool,        tools);
   MENU_ADD_TOOL( dbseg_seg_label_region_tool,        tools);
   MENU_ADD_TOOL( dbseg_seed_regions_tool,        tools);
  vgui_menu image_tools_menu;
  //MENU_ADD_TOOL(dbvis1_image_intensity_inspector, image_tools_menu );

  vgui_menu vsol2D_tools_menu, vsol2D_draw_tools_menu;
    /*MENU_ADD_TOOL( bvis1_vsol2D_line_tool,           vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_point_tool,          vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_polyline_tool,       vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_polygon_tool,        vsol2D_draw_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_digital_curve_tool,  vsol2D_draw_tools_menu );*/

  vsol2D_tools_menu.add("Drawing", vsol2D_draw_tools_menu);
    /*MENU_ADD_TOOL( bvis1_vsol2D_split_curve_tool,    vsol2D_tools_menu );
    MENU_ADD_TOOL( bvis1_vsol2D_crop_tool,           vsol2D_tools_menu);
    MENU_ADD_TOOL( bvis1_vsol2D_selection_tool,      vsol2D_tools_menu);*/
    
    //MENU_ADD_TOOL( bvis1_vsol2D_rotate_line_tool,    vsol2D_tools_menu );
    //MENU_ADD_TOOL( bvis1_vsol2D_intersect_curve_tool,vsol2D_tools_menu );
    //MENU_ADD_TOOL( bvis1_correspond_point_tool,      vsol2D_tools_menu );
    //MENU_ADD_TOOL( dbvis1_edge_selection_tool,       vsol2D_tools_menu );
    //MENU_ADD_TOOL( dbvis1_edge_transform_tool,       vsol2D_tools_menu );
    //MENU_ADD_TOOL( dbvis1_classify_curve_tool,       vsol2D_tools_menu );

  vgui_menu soview2D_tools_menu;
    /*MENU_ADD_TOOL( bvis1_translate_tool, soview2D_tools_menu );
    MENU_ADD_TOOL( bvis1_style_tool,     soview2D_tools_menu );*/

    


  tools.add("vsol2D",      vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);

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

  return MainMenu;
}




