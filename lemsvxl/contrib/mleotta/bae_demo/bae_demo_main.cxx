// GUI main program for bae_demo

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h> 
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "bae_demo_menu.h"
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
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <dbbgm/pro/dbbgm_image_storage.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include <dbbgm/vis/dbbgm_distribution_image_displayer.h>
#include <dbbgm/vis/dbbgm_image_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>
#include <vidpro1/process/vidpro1_save_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_vsol_scale_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_resample_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_motion_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>
#include <vidpro1/process/vidpro1_equalize_images_process.h>
//#include <dbil/pro/dbil_blob_tracker_process.h>
#include <dbsol/pro/dbsol_scan_polygon_process.h>
#include <dbbgm/pro/dbbgm_model_process.h>
#include <dbbgm/pro/dbbgm_init_model_process.h>
#include <dbbgm/pro/dbbgm_truth_model_process.h>
#include <dbbgm/pro/dbbgm_shadow_detect_process.h>
#include <dbbgm/pro/dbbgm_bg_detect_process.h>
#include <dbbgm/pro/dbbgm_hmm_train_process.h>
#include <dbbgm/pro/dbbgm_hmm_learn_trans_process.h>
#include <dbbgm/pro/dbbgm_hmm_detect_process.h>
#include <dbbgm/pro/dbbgm_equalize_process.h>
#include <modrec/pro/modrec_position_process.h>
#include <modrec/pro/modrec_classify_process.h>
#include <modrec/pro/modrec_evaluate_process.h>
#include <dvpgl/algo/pro/dvpgl_vsol_lens_warp_process.h>
#include <dvpgl/algo/pro/dvpgl_image_lens_warp_process.h>




int main(int argc, char** argv)
{
  vgui::init(argc, argv);

  // Register the displayer
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  REG_DISPLAYER( dbbgm_distribution_image_displayer );
  REG_DISPLAYER( dbbgm_image_displayer );


  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( dbbgm_distribution_image_storage );
  REG_STORAGE( dbbgm_image_storage );

  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_save_image_process );
  REG_PROCESS( vidpro1_save_video_process );
  REG_PROCESS( vidpro1_brightness_contrast_process );
  REG_PROCESS( vidpro1_VD_edge_process );
  REG_PROCESS( vidpro1_vsol_scale_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_resample_process );
  REG_PROCESS( vidpro1_grey_image_process );
  REG_PROCESS( vidpro1_frame_diff_process );
  REG_PROCESS( vidpro1_motion_process );
  REG_PROCESS( vidpro1_equalize_images_process );
//  REG_PROCESS( dbil_blob_tracker_process );
  REG_PROCESS( dbsol_scan_polygon_process);
  REG_PROCESS( dbbgm_model_process );
  REG_PROCESS( dbbgm_init_model_process );
  REG_PROCESS( dbbgm_truth_model_process );
  REG_PROCESS( dbbgm_shadow_detect_process );
  REG_PROCESS( dbbgm_bg_detect_process );
  REG_PROCESS( dbbgm_hmm_train_process );
  REG_PROCESS( dbbgm_hmm_learn_trans_process );
  REG_PROCESS( dbbgm_hmm_detect_process );
  REG_PROCESS( dbbgm_equalize_process );
  REG_PROCESS( modrec_position_process );
  REG_PROCESS( modrec_classify_process );
  REG_PROCESS( modrec_evaluate_process );
  REG_PROCESS( dvpgl_vsol_lens_warp_process );
  REG_PROCESS( dvpgl_image_lens_warp_process );


  bae_demo_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "BAE Shadow Demo GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}
