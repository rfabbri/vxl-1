// This is contrib/fine/gui/seg_gui_main.cxx

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

#include "seg_gui_menu.h"

#include <bvis1/bvis1_macros.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_displayer_sptr.h>
#include <vidpro1/vidpro1_process_manager_sptr.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/vidpro1_repository.h>

// Storage type header files
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage.h>

#include <structure/dbseg_seg_storage.h>


// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>

#include <vis/dbseg_seg_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_RGB_to_IHS_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_motion_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>

#include <vidpro1/process/vidpro1_background_diff_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/process/vidpro1_load_edg_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_edgeprune_process.h>
#include <vidpro1/process/vidpro1_random_del_polygon_process.h>


#include <vidpro1/process/vidpro1_load_image_sequence_process.h>

#include <dbmsh3d/algovtk/pro/dbmsh3d_isosurface_process.h>


#include <dbil/pro/dbil_wshed2d_process.h>

#include <pro/dbseg_seg_process.h>
#include <vis/dbseg_seg_create_region_tool.h>
#include <vis/dbseg_seg_edit_region_tool.h>
#include <vis/dbseg_seg_label_region_tool.h>

#include <pro/dbseg_seg_save_process.h>
#include <pro/dbseg_seg_save_view_process.h>
#include <pro/dbseg_seg_save_contour_process.h>
#include <pro/dbseg_seg_load_process.h>
#include <pro/dbseg_seg_save_binimage_process.h>
#include <pro/dbseg_seg_load_sequence_process.h>

#include <pro/dbseg_mean_shift_process.h>
#include <pro/dbseg_graph_process.h>
#include <pro/dbseg_jseg_process.h>
//#include <pro/dbseg_subpixel_process.h>
#include <pro/dbseg_srgtek_process.h>
//#include <pro/dbseg_levelset_process.h>


#include <dbacm/pro/dbacm_geodesic_active_contour_process.h>

#include <pro/dbseg_disk_measurements_process.h>
#include <pro/dbseg_disk_bin_images_process.h>


int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  //int b = dbseg_jseg_process::static_execute(4);
  // Register the displayers
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );

  REG_DISPLAYER( dbseg_seg_displayer );
  
  
  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );

  REG_STORAGE( dbseg_seg_storage );
  
  
  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_save_image_process );
  REG_PROCESS( vidpro1_load_video_process );

    REG_PROCESS( vidpro1_load_image_sequence_process );
    
    REG_PROCESS( dbmsh3d_isosurface_process );
    
    REG_PROCESS( dbseg_seg_save_process );
    REG_PROCESS( dbseg_seg_save_view_process );
    REG_PROCESS( dbseg_seg_load_process);
    REG_PROCESS( dbseg_seg_save_contour_process);
    REG_PROCESS( dbseg_seg_save_binimage_process);
    REG_PROCESS( dbseg_seg_load_sequence_process);

    REG_PROCESS( dbseg_disk_measurements_process);
    REG_PROCESS( dbseg_disk_bin_images_process);

    REG_PROCESS( dbseg_mean_shift_process);
    REG_PROCESS( dbseg_graph_process);
    REG_PROCESS( dbseg_jseg_process);
    //REG_PROCESS( dbseg_subpixel_process);
    REG_PROCESS( dbseg_srgtek_process);
    //REG_PROCESS( dbseg_levelset_process);

    REG_PROCESS( dbacm_geodesic_active_contour_process );

  /*REG_PROCESS( vidpro1_brightness_contrast_process );
  REG_PROCESS( vidpro1_RGB_to_IHS_process );
  REG_PROCESS( vidpro1_VD_edge_process );
  REG_PROCESS( vidpro1_harris_corners_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_grey_image_process ); 
  REG_PROCESS( vidpro1_frame_diff_process );  
  REG_PROCESS( vidpro1_motion_process );
  REG_PROCESS( vidpro1_background_diff_process );
  REG_PROCESS( vidpro1_load_con_process );
  REG_PROCESS( vidpro1_load_edg_process );
  REG_PROCESS( vidpro1_load_cem_process );
  REG_PROCESS( vidpro1_save_cem_process );
  REG_PROCESS( vidpro1_save_con_process );
  REG_PROCESS( vidpro1_edgeprune_process );
  REG_PROCESS( vidpro1_random_del_polygon_process );*/

  REG_PROCESS( dbil_wshed2d_process );

    REG_PROCESS( dbseg_seg_process );
  


  // --- other processes ---


  //REG_PROCESS( superimpose_frames_process );

  seg_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu(); 
  unsigned w = 512, h = 512;
  vcl_string title = "Seg 2D GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );

  //Amir:: The status bar operations are confusing and often unpredictable
  //until we can get it right, I think it's best to get used to the Ctrl-middlemousebutton
  //to pan the views instead of using the scroll bars

  //win->set_statusbar(true);
  //win->enable_vscrollbar(true);
  //win->enable_hscrollbar(true);
  win->show();
  return vgui::run(); 
}





