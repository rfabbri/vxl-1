// GUI main program for dbsksp_gui

#include "dbsksp_small_gui_menu.h"

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
//#include <dbsk2d/pro/dbsk2d_shock_storage.h>
//#include <dbskr/pro/dbskr_shock_match_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>
//#include <dbcvr/pro/dbcvr_curvematch_storage.h>
//#include <dbdet/pro/dbdet_sel_storage.h>
//#include <dbdet/pro/dbdet_edgemap_storage.h>



// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include <dbsk2d/vis/dbsk2d_shock_displayer.h>
//#include <dbskr/vis/dbskr_shock_match_displayer.h>
#include <dbsksp/vis/dbsksp_shock_displayer.h>
#include <dbsksp/vis/dbsksp_xgraph_displayer.h>
//#include <dbcvr/vis/dbcvr_curvematch_displayer.h>
//#include <dbdet/vis/dbdet_edgemap_displayer.h>
//#include <dbdet/vis/dbdet_sel_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_image_sequence_process.h>
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
//#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_edgeprune_process.h>

#include <dbsol/pro/dbsol_translate_2d_process.h>
#include <dbsol/pro/dbsol_save_ps_file_process.h>
#include <dbsol/pro/dbsol_fit_circ_arc_spline_process.h>



#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_prune_ishock_process.h>
//#include <dbsk2d/pro/dbsk2d_gap_transform_process.h>
#include <dbsk2d/pro/dbsk2d_sample_ishock_process.h>
//#include <dbsk2d/pro/dbsk2d_compile_rich_map_process.h>
//#include <dbsk2d/pro/dbsk2d_assign_image_process.h>


//#include <dbsk2d/pro/dbsk2d_load_bnd_process.h>
//#include <dbsk2d/pro/dbsk2d_save_bnd_process.h>
#include <dbsk2d/pro/dbsk2d_load_esf_process.h>
#include <dbsk2d/pro/dbsk2d_save_esf_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>


//#include <dbskr/pro/dbskr_shock_match_process.h>


#include <dbsksp/pro/dbsksp_save_shock_graph_xml_process.h>
#include <dbsksp/pro/dbsksp_load_shock_graph_xml_process.h>
#include <dbsksp/pro/dbsksp_trace_shock_boundary_process.h>
#include <dbsksp/pro/dbsksp_convert_shock_graph_to_xshock_graph_process.h>
#include <dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h>

#include <dbsksp/pro/dbsksp_build_shock_graph_process.h>
#include <dbsksp/pro/dbsksp_shock_snake_process.h>
#include <dbsksp/pro/dbsksp_deformable_shock_process.h>
#include <dbsksp/pro/dbsksp_morph_shock_graph_process.h>
#include <dbsksp/pro/dbsksp_morph_shock_graph_different_topology_process.h>
#include <dbsksp/pro/dbsksp_build_from_xshock_graph_process.h>

#include <dbsksp/pro/dbsksp_edit_distance_process.h>
#include <dbsksp/pro/dbsksp_average_two_xgraphs_process.h>
#include <dbsksp/pro/dbsksp_resample_xgraph_process.h>


//#include <dbsks/pro/dbsks_compute_symmetry_points_process.h>
//#include <dbsks/pro/dbsks_compute_shock_graph_stats_process.h>
//#include <dbsks/pro/dbsks_detect_shape_process.h>
//#include <dbsks/pro/dbsks_compute_arc_cost_process.h>
//#include <dbsks/pro/dbsks_save_arc_cost_process.h>
//#include <dbsks/pro/dbsks_load_arc_cost_process.h>
//#include <dbsks/pro/dbsks_save_dp_results_process.h>
//#include <dbsks/pro/dbsks_load_dp_results_process.h>
//#include <dbsks/pro/dbsks_trace_opt_graph_bnd_process.h>
//#include <dbsks/pro/dbsks_local_match_process.h>
//#include <dbsks/pro/dbsks_write_shapematch_to_ps_process.h>


//#include <dbdet/pro/dbdet_generic_edge_detector_process.h>
//#include <dbdet/pro/dbdet_generic_color_edge_detector_process.h>
//#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
//#include <dbdet/pro/dbdet_generic_linker_process.h>
//
//#include <dbdet/pro/dbdet_mask_edges_process.h>
//#include <dbdet/pro/dbdet_prune_curves_process.h>
//#include <dbdet/pro/dbdet_sel_process.h>
//#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
//#include <dbdet/pro/dbdet_image_gradient_process.h>
//#include <dbdet/pro/dbdet_convert_edgemap_to_image_process.h>

#include <dbsol/pro/dbsol_roi_image_process.h>
#include <dbdet/pro/dbdet_load_cem_process.h>

//FIRAT EDIT

#include <dbsks/pro/dbsks_train_xshock_geom_model_process.h>
#include <dbsks/pro/dbsks_train_xshock_ccm_model_process.h>
//#include <dbsksp/pro/dbsksp_edit_xgraph_to_common_topology_process.h>

//#include <dbsksp/pro/dbsksp_transfer_xgraph_attributes_process.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  REG_DISPLAYER( dbsk2d_shock_displayer );
  //REG_DISPLAYER( dbskr_shock_match_displayer );
  REG_DISPLAYER( dbsksp_shock_displayer );
  REG_DISPLAYER( dbsksp_xgraph_displayer );
  //REG_DISPLAYER( dbcvr_curvematch_displayer );
  //REG_DISPLAYER( dbdet_edgemap_displayer );
  //REG_DISPLAYER( dbdet_sel_displayer );

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( dbsk2d_shock_storage );
  //REG_STORAGE( dbskr_shock_match_storage );
  REG_STORAGE( dbsksp_shock_storage );
  REG_STORAGE( dbsksp_xgraph_storage );
  //REG_STORAGE( dbcvr_curvematch_storage );
  //REG_STORAGE( dbdet_edgemap_storage );
  //REG_STORAGE( dbdet_sel_storage );
  //REG_STORAGE( dbsks_shapematch_storage );
  

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
  //REG_PROCESS ( dbsksp_edit_xgraph_to_common_topology_process);


  dbsksp_small_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "dbsksp_small_gui GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}

