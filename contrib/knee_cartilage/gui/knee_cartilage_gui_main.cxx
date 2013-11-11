// GUI main program for knee_cartilage_gui

#include "knee_cartilage_gui_menu.h"

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

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

// Displayer header files
//#include <bvis1/displayer/bvis1_image_displayer.h>
#include "dbknee_dicom_image_displayer.h"
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_motion_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/process/vidpro1_load_edg_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <dbsol/pro/dbsol_translate_2d_process.h>
#include <dbsol/pro/dbsol_roi_image_process.h>


// Knee cartilage
#include <dbknee/pro/dbknee_compute_cylinder_cs_process.h>
#include <dbknee/pro/dbknee_separate_inner_outer_surfaces_process.h>
#include <dbknee/pro/dbknee_compute_thickness_process.h>
#include <dbknee/pro/dbknee_region_thickness_process.h>
#include <dbknee/pro/dbknee_export_thickness_map_process.h>
#include <dbknee/pro/dbknee_surface_points_from_contours_process.h>
#include <dbknee/pro/dbknee_surface_mesh_process.h>
#include <dbknee/pro/dbknee_plot_cylinder_cs_process.h>
#include <dbknee/pro/dbknee_compute_tibia_cs_process.h>
#include <dbknee/pro/dbknee_crop_mesh_process.h>

#include <dbdet/pro/dbdet_load_seg3d_info_xml_process.h>
#include <dbdet/pro/dbdet_new_seg3d_info_xml_process.h>
#include <dbdet/pro/dbdet_save_seg3d_info_xml_process.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  

  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_brightness_contrast_process );
  REG_PROCESS( vidpro1_VD_edge_process );
  REG_PROCESS( vidpro1_harris_corners_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_grey_image_process );
  REG_PROCESS( vidpro1_frame_diff_process );
  REG_PROCESS( vidpro1_motion_process );
  
  REG_PROCESS( vidpro1_load_con_process );
  REG_PROCESS( vidpro1_load_edg_process );
  REG_PROCESS( vidpro1_load_cem_process );
  REG_PROCESS( vidpro1_save_cem_process );
  REG_PROCESS( vidpro1_save_con_process );
  REG_PROCESS( vidpro1_save_image_process );

  REG_PROCESS( dbsol_translate_2d_process );
  REG_PROCESS( dbsol_roi_image_process );

  REG_PROCESS( dbknee_compute_cylinder_cs_process );
  REG_PROCESS( dbknee_separate_inner_outer_surfaces_process );
  REG_PROCESS( dbknee_compute_thickness_process );
  REG_PROCESS( dbknee_region_thickness_process );
  REG_PROCESS( dbknee_export_thickness_map_process );
  REG_PROCESS( dbknee_surface_points_from_contours_process );
  REG_PROCESS( dbknee_surface_mesh_process );
  REG_PROCESS( dbknee_plot_cylinder_cs_process ); 
  REG_PROCESS( dbknee_compute_tibia_cs_process );
  REG_PROCESS( dbknee_crop_mesh_process );

  
  REG_PROCESS(dbdet_load_seg3d_info_xml_process);
  REG_PROCESS(dbdet_new_seg3d_info_xml_process);
  REG_PROCESS(dbdet_save_seg3d_info_xml_process);

  knee_cartilage_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "knee_cartilage_gui GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}

