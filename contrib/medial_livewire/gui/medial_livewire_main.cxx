// GUI main program for medial_livewire

#ifdef DEBUG
#undef DEBUG
#endif

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

#include "medial_livewire_menu.h"

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

#include <dbdet/pro/dbdet_seg3d_info_storage.h>

// Displayer header files
//#include <bvis1/displayer/bvis1_image_displayer.h>
#include "bvis1_dicom_image_displayer.h"
//#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include "dbknee_vsol2D_displayer.h"

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_motion_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>

#include <vidpro1/process/vidpro1_load_con_process.h>
#include <dbdet/pro/dbdet_load_seg3d_info_xml_process.h>
#include <dbdet/pro/dbdet_new_seg3d_info_xml_process.h>
#include <dbdet/pro/dbdet_save_seg3d_info_xml_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>

#include <dbsol/pro/dbsol_roi_image_process.h>
#include <dbsol/pro/dbsol_smooth_contours_process.h>
#include <dbsol/pro/dbsol_merge_storage_process.h>

//#include <dbmsh3d/algovtk/pro/dbmsh3d_isosurface_of_seg3d_process.h>
//#include <dbmsh3d/algovtk/pro/dbmsh3d_isosurface_process.h>






int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis1_dicom_image_displayer );
  //REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( dbknee_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( dbdet_seg3d_info_storage );
  
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

  
  REG_PROCESS(dbdet_load_seg3d_info_xml_process);
  REG_PROCESS(vidpro1_load_con_process);
  REG_PROCESS(dbdet_new_seg3d_info_xml_process);
  REG_PROCESS(dbdet_save_seg3d_info_xml_process);

  // REG_PROCESS(dbmsh3d_isosurface_of_seg3d_process);
  //REG_PROCESS(dbmsh3d_isosurface_process);
  REG_PROCESS( dbsol_roi_image_process );
  REG_PROCESS( dbsol_smooth_contours_process );
  REG_PROCESS( dbdet_contour_tracer_process );
  REG_PROCESS( dbsol_merge_storage_process );
  
  // load image
  vgui_image_tableau_new image_tab("F:/vision/projects/lemsvxl/src/contrib/medial_livewire/gui/lvwr_instructions.png");

  //Put the image tableaux into viewer
  vgui_viewer2D_tableau_new viewer(image_tab);

  vgui_poly_tableau_new poly;
  poly->add(bvis1_manager::instance(), 0.0f, 0.0f, 0.8f, 1.0f);
  poly->add(viewer, 0.8f, 0.0f, 0.2f, 1.0f);
  
  // Put the grid into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(poly);

  medial_livewire_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 640, h = 512;
  vcl_string title = "medial_livewire GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);



  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  //win->get_adaptor()->set_tableau( shell );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}
