#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
//#include <vgui/vgui_command.h>
//#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
//#include <vgui/vgui_find.h>
#include "mw_stereo_menu.h"
#include <mw/app/show_contours_process.h>
//#include <mw/app/mw_app.h>
//#include <mw/app/mw_data.h>
//#include <mw/app/ctspheres_app.h>
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
#include <dvpgl/pro/dvpgl_camera_storage.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_keypoint_storage.h>   
#include <dbkpr/pro/dbkpr_corr3d_storage.h>  
#include <mw/pro/mw_discrete_corresp_storage.h>
#include <mw/pro/mw_discrete_corresp_storage_3.h>
#include <dbdif/pro/dbdif_3rd_order_geometry_storage.h>


// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include <dbdet/vis/dbdet_sel_displayer.h>
#include <dbdet/vis/dbdet_edgemap_displayer.h>
#include <dbdet/vis/dbdet_keypoint_displayer.h>


// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>
#include <vidpro1/process/vidpro1_save_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_resample_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_edgeprune_process.h>
#include <vidpro1/process/vidpro1_vsol_scale_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>
#include <vidpro1/process/vidpro1_load_vsol_process.h>
#include <vidpro1/process/vidpro1_save_vsol_process.h>
#include <dbdet/pro/dbdet_load_cvlet_map_process.h>
#include <dbdet/pro/dbdet_save_cvlet_map_process.h>
#include <dbsol/pro/dbsol_smooth_contours_process.h>





//: register planar regions between images
//#include <dbvrl/pro/dbvrl_region_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <dbdet/pro/dbdet_generic_edge_detector_process.h>
#include <dbdet/pro/dbdet_lowe_keypoint_process.h>

#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_composite_edge_detection_process.h>
#include <dbdet/pro/dbdet_generic_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_generic_multiscale_edge_detector_process.h>
#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_generic_linker_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbdet/pro/dbdet_image_gradient_process.h>
#include <dbdet/pro/dbdet_detect_topographic_curves_process.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_load_edg_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <dbdet/pro/dbdet_load_cem_process.h>
#include <dbdet/pro/dbdet_save_cem_process.h>
#include <dbdet/pro/dbdet_convert_edgemap_to_image_process.h>
#include <dbil/pro/dbil_distance_transform_process.h>
#include <dbkpr/pro/dbkpr_bbf_match_process.h>
#include <dbkpr/pro/dbkpr_span_match_process.h>
#include <dbkpr/pro/dbkpr_reconstruct_process.h>
#include <mw/pro/dbkpr_frenet_reconstruct_process.h>
#include <dbkpr/pro/dbkpr_interp_depth_process.h>
//#include <dvpgl/algo/pro/dvpgl_vsol_lens_warp_process.h>
//#include <dvpgl/algo/pro/dvpgl_image_lens_warp_process.h>
#include <mw/pro/dvpgl_load_camera_process.h>
#include <mw/pro/mw_load_discrete_corresp_process.h>
#include <mw/pro/mw_save_discrete_corresp_process.h>
#include <dbdif/pro/dbdif_edgel_data_process.h>
#include <mw/pro/dbdet_combine_curves_process.h>
#include <mw/pro/dbkpr_projective_reconstruct_process.h>
#include <mw/pro/mw_synthetic_corresp_process.h>
#include <mw/pro/mw_project_cube_process.h>
#include <mw/pro/mw_project_polyline_process.h>
#include <mw/pro/dbmcs_project_curve_sketch_process.h>
#include <mw/pro/dbdet_compute_linked_curves_process.h>
#include <mw/pro/mw_extend_corresp_process.h>
#include <mw/pro/dbmcs_nearest_edgels_process.h>

#ifdef HAS_BGUI3D
#include <bgui3d/bgui3d.h>
#include <dbkpr/vis/dbkpr_corr3d_displayer.h>
#endif



int main(int argc, char** argv)
{

  vgui::init(argc, argv);

#ifdef HAS_BGUI3D
  bgui3d_init();
  //: Initialize specialized Coin3d nodes
  SoCurvel3D::initClass();
#endif


  // Register the displayers
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  REG_DISPLAYER( dbdet_edgemap_displayer );  
  REG_DISPLAYER( dbdet_sel_displayer );
  REG_DISPLAYER( dbdet_keypoint_displayer );
#ifdef HAS_BGUI3D
  REG_DISPLAYER( dbkpr_corr3d_displayer );
#endif

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( dvpgl_camera_storage );
  REG_STORAGE( dbdet_sel_storage );
  REG_STORAGE( dbdet_edgemap_storage );
  REG_STORAGE( dbdet_keypoint_storage );
  REG_STORAGE( dbkpr_corr3d_storage );
  REG_STORAGE( mw_discrete_corresp_storage );
  REG_STORAGE( mw_discrete_corresp_storage_3 );
  REG_STORAGE( dbdif_3rd_order_geometry_storage );

  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_save_image_process );
  REG_PROCESS( vidpro1_save_video_process );
  REG_PROCESS( vidpro1_brightness_contrast_process );
  REG_PROCESS( vidpro1_VD_edge_process );
  REG_PROCESS( vidpro1_vsol_scale_process );
  REG_PROCESS( vidpro1_harris_corners_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_resample_process );
  REG_PROCESS( vidpro1_grey_image_process ); 
  REG_PROCESS( vidpro1_load_cem_process );
  REG_PROCESS( vidpro1_save_cem_process );
  REG_PROCESS( vidpro1_edgeprune_process );
  REG_PROCESS( dbil_distance_transform_process);

  
  REG_PROCESS( dbsol_smooth_contours_process );
  REG_PROCESS( dbdet_lowe_keypoint_process );
  REG_PROCESS( dbdet_generic_edge_detector_process );
  REG_PROCESS( dbdet_sel_process );
  REG_PROCESS( dbdet_sel_extract_contours_process );
  REG_PROCESS( dbdet_compute_linked_curves_process );
  REG_PROCESS( dbdet_contour_tracer_process );
  REG_PROCESS( dbdet_image_gradient_process );
  REG_PROCESS( dbdet_third_order_edge_detector_process );
  REG_PROCESS( dbdet_composite_edge_detection_process );
  REG_PROCESS( dbdet_generic_color_edge_detector_process );
  REG_PROCESS( dbdet_third_order_color_edge_detector_process );
  REG_PROCESS( dbdet_generic_multiscale_edge_detector_process );
  REG_PROCESS( dbdet_generic_linker_process );
  REG_PROCESS( dbdet_convert_edgemap_to_image_process );
  
  REG_PROCESS( dbdet_detect_topographic_curves_process );
  REG_PROCESS( dbdet_load_edg_process );
  REG_PROCESS( dbdet_save_edg_process );
  REG_PROCESS( dbdet_load_cem_process );
  REG_PROCESS( dbdet_save_cem_process );
  REG_PROCESS( dbdet_load_cvlet_map_process );
  REG_PROCESS( dbdet_save_cvlet_map_process );

  REG_PROCESS( dbdet_prune_curves_process );

  REG_PROCESS( dbkpr_bbf_match_process );
  REG_PROCESS( dbkpr_span_match_process );
  REG_PROCESS( dbkpr_reconstruct_process );
  REG_PROCESS( dbkpr_frenet_reconstruct_process );
  REG_PROCESS( dbkpr_projective_reconstruct_process );
  REG_PROCESS( dbkpr_interp_depth_process );
//  REG_PROCESS( dvpgl_vsol_lens_warp_process );
//  REG_PROCESS( dvpgl_image_lens_warp_process );
  REG_PROCESS( dvpgl_load_camera_process );
  REG_PROCESS( show_contours_process );

  REG_PROCESS( mw_load_discrete_corresp_process );
  REG_PROCESS( mw_save_discrete_corresp_process );
  REG_PROCESS( vidpro1_load_vsol_process );
  REG_PROCESS( vidpro1_save_vsol_process );
  REG_PROCESS( dbdif_edgel_data_process );
  REG_PROCESS( dbdet_combine_curves_process );
  REG_PROCESS( mw_synthetic_corresp_process );
  REG_PROCESS( mw_project_cube_process );
  REG_PROCESS( mw_project_polyline_process );
  REG_PROCESS( dbmcs_project_curve_sketch_process );
  REG_PROCESS( dbmcs_nearest_edgels_process );
  REG_PROCESS( mw_extend_corresp_process );

  mw_stereo_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu(); 
//  unsigned w = 3200, h = 1000;
  unsigned w = 1200, h = 700;
  vcl_string title = "Brown Eyes";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  
  win->show();

  return vgui::run(); 
}
