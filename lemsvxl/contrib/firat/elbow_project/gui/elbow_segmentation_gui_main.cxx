// GUI main program for dbsksp_gui

#include "elbow_segmentation_gui_menu.h"

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
#include "../storage/elbow_vil3d_storage.h"
#include "../storage/dbdet_third_order_3d_edge_storage.h"
#include "../storage/elbow_mesh_file_storage.h"
#include "../storage/elbow_levelset_function_storage.h"


// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include "../pro/elbow_load_3d_volume_process.h"
#include "../pro/elbow_show_slices_process.h"
#include "../pro/dbdet_third_order_3d_edge_detector_process.h"
#include "../pro/elbow_get_largest_edge_cluster_process.h"
#include "../pro/poisson_reconstruction_process.h"
#include "../pro/elbow_mesh_post_process.h"
#include "../pro/elbow_show_3d_mesh_process.h"
#include "../pro/dbdet_3d_edge_save_process.h"
#include "../pro/dbdet_3d_edge_load_process.h"
#include "../pro/dbdet_third_order_3d_edge_detector_threaded_process.h"
#include "../pro/dbdet_graddt_levelset2d_volume_segmentation_process.h"

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
  REG_STORAGE( elbow_vil3d_storage );
  REG_STORAGE( dbdet_third_order_3d_edge_storage );
  REG_STORAGE( elbow_mesh_file_storage );
  REG_STORAGE( elbow_levelset_function_storage );


  // Register all the processes
  REG_PROCESS( elbow_load_3d_volume_process );
  REG_PROCESS( elbow_show_slices_process );
  REG_PROCESS( dbdet_third_order_3d_edge_detector_process );
  REG_PROCESS( elbow_get_largest_edge_cluster_process );
  REG_PROCESS( poisson_reconstruction_process );
  REG_PROCESS( elbow_mesh_post_process );
  REG_PROCESS( elbow_show_3d_mesh_process );
  REG_PROCESS( dbdet_3d_edge_save_process );
  REG_PROCESS( dbdet_3d_edge_load_process );
  REG_PROCESS( dbdet_third_order_3d_edge_detector_threaded_process );
  REG_PROCESS( dbdet_graddt_levelset2d_volume_segmentation_process );

  //REG_PROCESS( vidpro1_load_image_process );
  /*REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_brightness_contrast_process );
  REG_PROCESS( vidpro1_VD_edge_process );
  REG_PROCESS( vidpro1_harris_corners_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_grey_image_process );
  REG_PROCESS( vidpro1_frame_diff_process );
  REG_PROCESS( vidpro1_motion_process );

  REG_PROCESS( vidpro1_edgeprune_process );
  
  REG_PROCESS( vidpro1_load_con_process );
  REG_PROCESS( vidpro1_load_edg_process );
  //REG_PROCESS( vidpro1_load_cem_process );
  REG_PROCESS( dbdet_load_cem_process );
  REG_PROCESS( vidpro1_save_cem_process );
  REG_PROCESS( vidpro1_save_con_process );
  REG_PROCESS( vidpro1_save_image_process );

  REG_PROCESS( vidpro1_load_image_sequence_process );
  REG_PROCESS( dbsol_translate_2d_process );

  //REG_PROCESS( dbsk2d_load_bnd_process );
  //REG_PROCESS( dbsk2d_save_bnd_process );
  REG_PROCESS( dbsk2d_compute_ishock_process );
  REG_PROCESS( dbsk2d_prune_ishock_process );
  //REG_PROCESS( dbsk2d_gap_transform_process );
  REG_PROCESS( dbsk2d_sample_ishock_process );
  //REG_PROCESS( dbsk2d_compile_rich_map_process );
  //REG_PROCESS( dbsk2d_assign_image_process );
  REG_PROCESS( dbsk2d_load_esf_process );
  REG_PROCESS( dbsk2d_save_esf_process );
  REG_PROCESS( vidpro1_load_cem_process );

  //REG_PROCESS( dbskr_shock_match_process );

  REG_PROCESS( dbsol_roi_image_process );
  REG_PROCESS( dbsol_save_ps_file_process );
  REG_PROCESS( dbsol_fit_circ_arc_spline_process );

  // shock shape
  REG_PROCESS( dbsksp_build_shock_graph_process );
  REG_PROCESS( dbsksp_shock_snake_process );
  REG_PROCESS( dbsksp_deformable_shock_process );
  REG_PROCESS( dbsksp_trace_shock_boundary_process );
  REG_PROCESS( dbsksp_save_shock_graph_xml_process );
  REG_PROCESS( dbsksp_load_shock_graph_xml_process );
  REG_PROCESS( dbsksp_morph_shock_graph_process );
  REG_PROCESS( dbsksp_morph_shock_graph_different_topology_process );
  REG_PROCESS( dbsksp_build_from_xshock_graph_process );
  REG_PROCESS( dbsksp_convert_shock_graph_to_xshock_graph_process );
  REG_PROCESS( dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process );
  REG_PROCESS( dbsksp_average_two_xgraphs_process );
  //REG_PROCESS( dbsksp_transfer_xgraph_attributes_process );

  REG_PROCESS( dbsksp_edit_distance_process );
  REG_PROCESS( dbsksp_resample_xgraph_process );


  //// symmetry-based segmentation
  //REG_PROCESS( dbsks_compute_symmetry_points_process );
  //REG_PROCESS( dbsks_detect_shape_process );
  //REG_PROCESS( dbsks_compute_arc_cost_process );
  //REG_PROCESS( dbsks_save_arc_cost_process );
  //REG_PROCESS( dbsks_load_arc_cost_process );
  //REG_PROCESS( dbsks_save_dp_results_process );
  //REG_PROCESS( dbsks_load_dp_results_process );
  //REG_PROCESS( dbsks_trace_opt_graph_bnd_process );
  //REG_PROCESS( dbsks_local_match_process );
  //REG_PROCESS( dbsks_write_shapematch_to_ps_process );
  //REG_PROCESS( dbsks_compute_shock_graph_stats_process );
  
  //// curve matching
  //REG_PROCESS( dbcvr_curvematch_process );
  //REG_PROCESS( dbcvr_interp_cvmatch_process );
  //
  // edge detection
  REG_PROCESS( dbdet_contour_tracer_process );
  //REG_PROCESS( dbdet_generic_edge_detector_process );
  //REG_PROCESS( dbdet_generic_color_edge_detector_process );
  //REG_PROCESS( dbdet_third_order_edge_detector_process );
  //REG_PROCESS( dbdet_third_order_color_edge_detector_process );
  //REG_PROCESS( dbdet_generic_linker_process );
  //REG_PROCESS( dbdet_mask_edges_process );
  //REG_PROCESS( dbdet_prune_curves_process );
  //REG_PROCESS( dbdet_sel_process );
  //REG_PROCESS( dbdet_image_gradient_process );
  //REG_PROCESS( dbdet_convert_edgemap_to_image_process );

  REG_PROCESS( dbsks_train_xshock_geom_model_process );
  REG_PROCESS( dbsks_train_xshock_ccm_model_process );
  //REG_PROCESS ( dbsksp_edit_xgraph_to_common_topology_process);*/


  elbow_segmentation_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "Elbow Segmentation GUI - LEMS - Brown University - 2011";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}

