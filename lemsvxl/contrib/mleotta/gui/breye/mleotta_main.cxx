// GUI main program for mleotta

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h> 
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "mleotta_menu.h"
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
//#include <dbmrf/pro/dbmrf_bmrf_storage.h>
//#include <dbmrf/pro/dbmrf_curvel_3d_storage.h>
#include <dbvrl/pro/dbvrl_region_storage.h>
#include <dbecl/pro/dbecl_episeg_storage.h>
#include <dbetl/pro/dbetl_track_storage.h>
#include <dbdet/pro/dbdet_keypoint_storage.h>   
#include <dbkpr/pro/dbkpr_corr3d_storage.h>  
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <dbbgm/pro/dbbgm_image_storage.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
//#include <dbmrf/vis/dbmrf_bmrf_displayer.h>
#include <dbvrl/vis/dbvrl_region_displayer.h>
#include <dbecl/vis/dbecl_episeg_displayer.h>
#include <dbdet/vis/dbdet_keypoint_displayer.h>
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
#include <vidpro1/process/vidpro1_horn_schunck_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>
#include <vidpro1/process/vidpro1_equalize_images_process.h>
#include <dbil/pro/dbil_harr_wavelet_process.h>
#include <dbil/pro/dbil_blob_tracker_process.h>
#include <dbsol/pro/dbsol_scan_polygon_process.h>
//#include <dbmrf/pro/dbmrf_network_builder_process.h>
//#include <dbmrf/pro/dbmrf_predict_process.h>
//#include <dbmrf/pro/dbmrf_evaluate_process.h>
//#include <dbmrf/pro/dbmrf_backproject_process.h>
//#include <dbmrf/pro/dbmrf_rocdata_process.h>
//#include <dbmrf/pro/dbmrf_curve_3d_builder_process.h>
//#include <dbmrf/pro/dbmrf_curve_projector_process.h>
#include <dbvrl/pro/dbvrl_video_process.h>
#include <dbvrl/pro/dbvrl_region_process.h>
#include <dbecl/pro/dbecl_builder_process.h>
#include <dbetl/pro/dbetl_tracker_process.h>
#include <dbdet/pro/dbdet_lowe_keypoint_process.h>
#include <dbkpr/pro/dbkpr_bbf_match_process.h>
#include <dbkpr/pro/dbkpr_span_match_process.h>
#include <dbkpr/pro/dbkpr_reconstruct_process.h>
#include <dbkpr/pro/dbkpr_interp_depth_process.h>
#include <dbbgm/pro/dbbgm_model_process.h>
#include <dbbgm/pro/dbbgm_init_model_process.h>
//#include <dbbgm/pro/dbbgm_joint_model_process.h>
#include <dbbgm/pro/dbbgm_truth_model_process.h>
#include <dbbgm/pro/dbbgm_shadow_detect_process.h>
#include <dbbgm/pro/dbbgm_bg_detect_process.h>
//#include <dbbgm/pro/dbbgm_joint_detect_process.h>
#include <dbbgm/pro/dbbgm_hmm_train_process.h>
#include <dbbgm/pro/dbbgm_hmm_learn_trans_process.h>
#include <dbbgm/pro/dbbgm_hmm_detect_process.h>
#include <dbbgm/pro/dbbgm_equalize_process.h>
#include <modrec/pro/modrec_position_process.h>
#include <modrec/pro/modrec_classify_process.h>
#include <modrec/pro/modrec_evaluate_process.h>
#include <modrec/pro/modrec_depthmap_process.h>
//#include <dbsta/pro/dbsta_model_image_process.h>
//#include <dbsta/pro/dbsta_model_image_regions_process.h>
#include <dvpgl/algo/pro/dvpgl_vsol_lens_warp_process.h>
#include <dvpgl/algo/pro/dvpgl_image_lens_warp_process.h>
//#include <dvpgl/algo/pro/dvpgl_epipolar_param_process.h>


#include <mleotta/pro/mleotta_video_slice_process.h>
#include <mleotta/pro/mleotta_save_keypoints_process.h>



#ifdef HAS_BGUI3D
#include <bgui3d/bgui3d.h>
//#include <dbmrf/vis/SoCurvel3D.h>
#include <dbetl/vis/dbetl_track_displayer.h>
//#include <dbmrf/vis/dbmrf_curvel_3d_displayer.h>
#include <dbkpr/vis/dbkpr_corr3d_displayer.h>
#endif


int main(int argc, char** argv)
{
  vgui::init(argc, argv);
#ifdef HAS_BGUI3D
  bgui3d_init();
  //: Initialize specialized Coin3d nodes
  //SoCurvel3D::initClass();
#endif
  // Register the displayer
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  //REG_DISPLAYER( dbmrf_bmrf_displayer );
  REG_DISPLAYER( dbvrl_region_displayer );
  REG_DISPLAYER( dbecl_episeg_displayer );
  REG_DISPLAYER( dbdet_keypoint_displayer );
  REG_DISPLAYER( dbbgm_distribution_image_displayer );
  REG_DISPLAYER( dbbgm_image_displayer );
#ifdef HAS_BGUI3D
  REG_DISPLAYER( dbetl_track_displayer );
  //REG_DISPLAYER( dbmrf_curvel_3d_displayer );
  REG_DISPLAYER( dbkpr_corr3d_displayer );
#endif

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  //REG_STORAGE( dbmrf_bmrf_storage );
  //REG_STORAGE( dbmrf_curvel_3d_storage );
  REG_STORAGE( dbvrl_region_storage );
  REG_STORAGE( dbecl_episeg_storage );
  REG_STORAGE( dbetl_track_storage );
  REG_STORAGE( dbdet_keypoint_storage );
  REG_STORAGE( dbkpr_corr3d_storage );
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
  REG_PROCESS( vidpro1_harris_corners_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_resample_process );
  REG_PROCESS( vidpro1_grey_image_process );
  REG_PROCESS( vidpro1_frame_diff_process );
  REG_PROCESS( vidpro1_motion_process );
  REG_PROCESS( vidpro1_horn_schunck_process );
  REG_PROCESS( vidpro1_equalize_images_process );
  REG_PROCESS( dbil_harr_wavelet_process );
  REG_PROCESS( dbil_blob_tracker_process );
  REG_PROCESS( dbsol_scan_polygon_process);
  //REG_PROCESS( dbmrf_network_builder_process );
  //REG_PROCESS( dbmrf_predict_process );
  //REG_PROCESS( dbmrf_evaluate_process );
  //REG_PROCESS( dbmrf_backproject_process );
  //REG_PROCESS( dbmrf_rocdata_process );
  //REG_PROCESS( dbmrf_curve_3d_builder_process );
  //REG_PROCESS( dbmrf_curve_projector_process );
  REG_PROCESS( dbecl_builder_process );
  REG_PROCESS( dbetl_tracker_process );
  REG_PROCESS( dbvrl_video_process );
  REG_PROCESS( dbvrl_region_process );
  REG_PROCESS( dbdet_lowe_keypoint_process );
  REG_PROCESS( dbkpr_bbf_match_process );
  REG_PROCESS( dbkpr_span_match_process );
  REG_PROCESS( dbkpr_reconstruct_process );
  REG_PROCESS( dbkpr_interp_depth_process );
  REG_PROCESS( dbbgm_model_process );
  REG_PROCESS( dbbgm_init_model_process );
//  REG_PROCESS( dbbgm_joint_model_process );
  REG_PROCESS( dbbgm_truth_model_process );
  REG_PROCESS( dbbgm_shadow_detect_process );
  REG_PROCESS( dbbgm_bg_detect_process );
//  REG_PROCESS( dbbgm_joint_detect_process );
  REG_PROCESS( dbbgm_hmm_train_process );
  REG_PROCESS( dbbgm_hmm_learn_trans_process );
  REG_PROCESS( dbbgm_hmm_detect_process );
  REG_PROCESS( dbbgm_equalize_process );
  REG_PROCESS( modrec_position_process );
  REG_PROCESS( modrec_classify_process );
  REG_PROCESS( modrec_evaluate_process );
  REG_PROCESS( modrec_depthmap_process );
//  REG_PROCESS( dbsta_model_image_process );
//  REG_PROCESS( dbsta_model_image_regions_process );
  REG_PROCESS( dvpgl_vsol_lens_warp_process );
  REG_PROCESS( dvpgl_image_lens_warp_process );
//  REG_PROCESS( dvpgl_epipolar_param_process );
  REG_PROCESS( mleotta_video_slice_process );
  REG_PROCESS( mleotta_save_keypoints_process );



  mleotta_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "mleotta GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}
