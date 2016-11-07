// This is contrib/shock2d/gui/shock2d_gui_main.cxx

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

#include "shock2d_gui_menu.h"

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
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbskr/pro/dbskr_shock_match_storage.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/pro/dbskr_shock_patch_match_storage.h>
#include <dbru/pro/dbru_rcor_storage.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dber/pro/dber_edge_match_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
//#include <pro/dbrl_match_set_storage.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include <dbsk2d/vis/dbsk2d_shock_displayer.h>
#include <dbskr/vis/dbskr_shock_match_displayer.h>
#include <dbskr/vis/dbskr_shock_patch_displayer.h>
#include <dbskr/vis/dbskr_shock_patch_match_displayer.h>
#include <dbru/vis/dbru_rcor_displayer.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/vis/dbdet_sel_displayer.h>
#include <dber/vis/dber_edge_match_displayer.h>
#include <dber/vis/dber_instance_displayer.h>
#include <dbdet/vis/dbdet_edgemap_displayer.h>
//#include <vis/dbrl_match_set_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
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
#include <dbsol/pro/dbsol_smooth_contours_process.h>

#include <dbil/pro/dbil_wshed2d_process.h>

#include <dbdet/pro/dbdet_contour_tracer_process.h>

#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_prune_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_gap_transform_process.h>
#include <dbsk2d/pro/dbsk2d_sample_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_compile_rich_map_process.h>
#include <dbsk2d/pro/dbsk2d_assign_image_process.h>

#include <dbsk2d/pro/dbsk2d_load_bnd_process.h>
#include <dbsk2d/pro/dbsk2d_save_bnd_process.h>
#include <dbsk2d/pro/dbsk2d_load_esf_process.h>
#include <dbsk2d/pro/dbsk2d_save_esf_process.h>
//#include <dbsk2d/pro/dbsk2d_visfrag_avg_intensity_process.h>

#include <dbskr/pro/dbskr_compute_sk_path_curve_process.h>
#include <dbskr/pro/dbskr_shock_match_process.h>
#include <dbskr/pro/dbskr_shock_match_pmi_process.h>
#include <dbskr/pro/dbskr_subshock_match_process.h>
#include <dbskr/pro/dbskr_shock_patch_match_process.h>
#include <dbskr/pro/dbskr_extract_shock_patches_process.h>

#include <dbskr/pro/dbskr_save_shock_patch_process.h>
#include <dbskr/pro/dbskr_load_shock_patch_process.h>
#include <dbskr/pro/dbskr_extract_subgraph_and_find_shock_patches_process.h>
#include <dbskr/pro/dbskr_detect_shock_patches_process.h>

#include <dbru/pro/dbru_dbinfo_process.h>
#include <dbru/pro/dbru_compute_rcor_process.h>
#include <dbru/pro/dbru_print_rcor_process.h>

#include <dbdet/pro/dbdet_generic_edge_detector_process.h>
#include <dbdet/pro/dbdet_generic_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>

#include <dbdet/pro/dbdet_mask_edges_process.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dber/pro/dber_edge_match_process.h>
#include <dber/pro/dber_edge_db_match_process.h>
#include <dber/pro/dber_load_instances_process.h>
#include <dber/pro/dber_save_instances_process.h>


//#include <pro/superimpose_frames_process.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);

  // Register the displayers
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  REG_DISPLAYER( dbsk2d_shock_displayer );
  REG_DISPLAYER( dbskr_shock_match_displayer );
  REG_DISPLAYER( dbru_rcor_displayer );
  REG_DISPLAYER( dbdet_sel_displayer );
  REG_DISPLAYER( dber_edge_match_displayer );
  REG_DISPLAYER( dber_instance_displayer );
  REG_DISPLAYER( dbdet_edgemap_displayer );
  //REG_DISPLAYER( dbrl_match_set_displayer );
  REG_DISPLAYER( dbskr_shock_patch_displayer );
  REG_DISPLAYER( dbskr_shock_patch_match_displayer );

  
  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( dbsk2d_shock_storage );
  REG_STORAGE( dbskr_shock_match_storage );
  REG_STORAGE( dbru_rcor_storage );
  REG_STORAGE( dbdet_sel_storage );
  REG_STORAGE( dber_edge_match_storage );
  REG_STORAGE( dbdet_edgemap_storage );
  REG_STORAGE( dber_instance_storage );
  //REG_STORAGE( dbrl_match_set_storage );
  REG_STORAGE( dbskr_shock_patch_storage );
  REG_STORAGE( dbskr_shock_patch_match_storage );

  
  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_brightness_contrast_process );
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
  REG_PROCESS( vidpro1_random_del_polygon_process );
  REG_PROCESS( dbsol_smooth_contours_process );

  // --- other processes ---
  REG_PROCESS( dbdet_contour_tracer_process );
  REG_PROCESS( dbdet_generic_edge_detector_process );
  REG_PROCESS( dbdet_generic_color_edge_detector_process );
  REG_PROCESS( dbdet_third_order_edge_detector_process );
  REG_PROCESS( dbdet_sel_process );

  REG_PROCESS( dbsk2d_load_bnd_process );
  REG_PROCESS( dbsk2d_save_bnd_process );
  REG_PROCESS( dbsk2d_compute_ishock_process );
  REG_PROCESS( dbsk2d_prune_ishock_process );
  REG_PROCESS( dbsk2d_gap_transform_process );
  REG_PROCESS( dbsk2d_sample_ishock_process );
  REG_PROCESS( dbsk2d_compile_rich_map_process );
  REG_PROCESS( dbsk2d_assign_image_process );
  //REG_PROCESS( dbsk2d_visfrag_avg_intensity_process );
  REG_PROCESS( dbsk2d_load_esf_process );
  REG_PROCESS( dbsk2d_save_esf_process );
  REG_PROCESS( dbil_wshed2d_process );
  
  REG_PROCESS( dbskr_compute_sk_path_curve_process );
  REG_PROCESS( dbskr_shock_match_process );
  REG_PROCESS( dbskr_shock_match_pmi_process );
  REG_PROCESS( dbskr_subshock_match_process );
  REG_PROCESS( dbskr_shock_patch_match_process );
  REG_PROCESS( dbskr_extract_shock_patches_process );
  REG_PROCESS( dbskr_load_shock_patch_process );
  REG_PROCESS( dbskr_save_shock_patch_process );
  REG_PROCESS( dbskr_extract_subgraph_and_find_shock_patches_process );
  REG_PROCESS( dbskr_detect_shock_patches_process );

  REG_PROCESS( dbru_dbinfo_process );
  REG_PROCESS( dbru_compute_rcor_process );
  REG_PROCESS( dbru_print_rcor_process );
  REG_PROCESS( dbdet_generic_edge_detector_process );
  REG_PROCESS( dbdet_third_order_color_edge_detector_process );
  REG_PROCESS( dbdet_mask_edges_process );
  REG_PROCESS( dbdet_prune_curves_process );
  REG_PROCESS( dber_edge_match_process );
  REG_PROCESS( dber_edge_db_match_process );
  REG_PROCESS( dber_load_instances_process );
  REG_PROCESS( dber_save_instances_process );
  //REG_PROCESS( superimpose_frames_process );

  shock2d_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu(); 
  unsigned w = 512, h = 512;
  vcl_string title = "Shock 2D GUI";
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

