// This is contrib/edgemodeling/gui/edgemodeling_gui_main.cxx

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

#include "georegister_gui_menu.h"

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
#include <dbvrl/pro/dbvrl_region_storage.h>

//#include <dbctrk/pro/dbctrk_pairing_curves_process.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include <dbsol/vis/dbsol_vsol2D_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_save_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_motion_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>
#include <vidpro1/process/vidpro1_background_diff_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
//#include <vidpro1/process/vidpro1_load_edg_process.h>
//#include <vidpro1/process/vidpro1_load_cem_process.h>
//#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_edgeprune_process.h>
#include <vidpro1/process/vidpro1_smoothcem_process.h>
#include <vidpro1/process/vidpro1_kl_process.h>
#include <pro/edgemodel_image_to_vsol_process.h>
#include <pro/dbil_osl_canny_subpixel_edges_process.h>  
#include <pro/dbrl_edge_point_tangent_bg_model_process.h> 
#include <pro/dbrl_edge_point_tangent_fg_detect_process.h>
#include <pro/dbrl_denoise_point_set_process.h>
#include <vis/dbrl_match_set_tableau.h>
#include <vis/dbrl_match_set_displayer.h>
#include <pro/dbrl_match_set_storage.h>
#include <pro/dbrl_rpm_rigid_process.h>
#include <pro/dbrl_rpm_affine_process.h>
#include <pro/dbrl_rpm_tps_process.h>
#include <pro/dbrl_ground_truth_point_set_generator.h>
#include <pro/dbrl_rpm_tps_superimpose_process.h>
#include <pro/dbrl_rpm_tps_affine_superimpose_process.h>
#include <pro/dbrl_rpm_affine_superimpose_process.h>
#include <pro/dbrl_rpm_tps_affine_local_global_process.h>
#include <pro/dbrl_compare_matchsets_process.h>
#include <pro/dbrl_rpm_tps_affine_superimpose_fgbg_process.h>
#include <pro/dbrl_warp_image_process.h>
#include <pro/superimpose_frames_process.h>
#include <pro/dbrl_support_process.h>
#include <pro/dbrl_support_subpixel_process.h>
#include <pro/dbrl_crop_id_points_process.h>
#include <pro/dbrl_superresolution_multiple_objects.h>
#include <pro/dbrl_postprocess_observation.h>
#include <pro/dbrl_superimpose_2ndtier.h>
#include <pro/dbrl_compute_amir_curvelets_process.h>
#include <pro/dbrl_load_multiple_instance_object_process.h>
#include <pro/dbrl_load_con_process.h>
#include <pro/dbrl_compute_motion_field_process.h>
//#include <pro/dbbgm_bgmodel_edges_process.h>
//#include <pro/dbbgm_fgdetect_edges_process.h>

#include <pro/dbrl_oriented_edgemap_to_edge_image_process.h>
#include <pro/dbrl_edge_image_to_oriented_edgemap_process.h>

#include <dbil/pro/dbil_wshed2d_process.h>
#include <pro/dbrl_id_point_2d_storage.h>
#include <vis/dbrl_id_point_2d_displayer.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <dbbgm/vis/dbbgm_distribution_image_displayer.h>
#include <dbbgm/pro/dbbgm_image_storage.h>
#include <dbbgm/vis/dbbgm_image_displayer.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process1.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process1.h>
#include <dbdet/pro/dbdet_blob_finder_process.h>
#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_generic_linker_process.h>
#include <dbinfo/pro/dbinfo_multi_track_process.h>
#include <dbinfo/pro/dbinfo_multi_poly_track_process.h>

#include <dbinfo/pro/dbinfo_load_tracks_process.h>
#include <dbinfo/pro/dbinfo_save_tracks_process.h>
#include <dbinfo/pro/dbinfo_load_vj_polys_process.h>
#include <dbinfo/vis/dbinfo_track_displayer.h>
#include <dbinfo/pro/dbinfo_track_storage.h>

#include <dbacm/pro/dbacm_geodesic_active_contour_process.h>

#include <dbbgm/pro/dbbgm_load_bg_model_process.h>
#include <dbbgm/pro/dbbgm_save_bg_model_process.h>
#include <dbbgm/pro/dbbgm_load_bg_model_process1.h>
#include <dbbgm/pro/dbbgm_save_bg_model_process1.h>
#include <dbsol/pro/dbsol_scan_polygon_process.h>
#include <dbdet/pro/dbdet_generic_edge_detector_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/vis/dbdet_sel_displayer.h>
#include <dbdet/vis/dbdet_edgemap_displayer.h>
#include <dbdet/pro/dbdet_save_cem_process.h>
#include <dbdet/pro/dbdet_save_cem_seq_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <dbdet/pro/dbdet_save_edg_seq_process.h>
#include <dbdet/pro/dbdet_load_cem_process.h>
#include <dbdet/pro/dbdet_load_edg_process.h>
#

#include <dbru/pro/dbru_load_polygons_process.h>

#include <vcl_cstdio.h>

int main(int argc, char** argv)
{

#if defined(VCL_WIN32)
    vcl_cout << '\n'<< "Max number of open files has been reset from " << _getmaxstdio();
    _setmaxstdio(2048);
#endif
    vgui::init(argc, argv);

    // Register the displayers
    REG_DISPLAYER( bvis1_image_displayer );
    REG_DISPLAYER( bvis1_vsol2D_displayer );
    //REG_DISPLAYER( dbsol_vsol2D_displayer );
    REG_DISPLAYER( bvis1_vtol_displayer );
    REG_DISPLAYER( dbrl_match_set_displayer );
    REG_DISPLAYER( dbrl_id_point_2d_displayer );
    REG_DISPLAYER( dbbgm_distribution_image_displayer );
    REG_DISPLAYER( dbbgm_image_displayer );
    REG_DISPLAYER( dbinfo_track_displayer );
    REG_DISPLAYER( dbdet_sel_displayer );
    REG_DISPLAYER( dbdet_edgemap_displayer );

    // Register the storage types
    REG_STORAGE( vidpro1_image_storage );
    REG_STORAGE( vidpro1_vsol2D_storage );
    REG_STORAGE( vidpro1_vtol_storage );
    REG_STORAGE( dbrl_match_set_storage );
    REG_STORAGE( dbbgm_distribution_image_storage );
    REG_STORAGE( dbbgm_image_storage );
    REG_STORAGE( dbrl_id_point_2d_storage );
    REG_STORAGE( dbinfo_track_storage );
    REG_STORAGE( dbdet_sel_storage );
    REG_STORAGE( dbdet_edgemap_storage );
    // --- dvidpro storage types ---
    REG_STORAGE( dbvrl_region_storage );
    // Register all the processes
    REG_PROCESS( vidpro1_load_image_process );
    REG_PROCESS( vidpro1_save_video_process );
    REG_PROCESS( vidpro1_load_video_process );
    REG_PROCESS( vidpro1_brightness_contrast_process );
    REG_PROCESS( vidpro1_VD_edge_process );
    REG_PROCESS( vidpro1_harris_corners_process );
    REG_PROCESS( vidpro1_gaussian_blur_process );
    REG_PROCESS( vidpro1_grey_image_process ); 
    REG_PROCESS( vidpro1_frame_diff_process );  
    REG_PROCESS( vidpro1_motion_process );
    REG_PROCESS( vidpro1_background_diff_process );
    REG_PROCESS( vidpro1_load_con_process );
    REG_PROCESS( dbdet_load_edg_process );
    REG_PROCESS( dbdet_load_cem_process );
    REG_PROCESS( dbdet_save_cem_process );
    REG_PROCESS( dbdet_save_cem_seq_process );
	REG_PROCESS( dbdet_save_edg_process );
	REG_PROCESS( dbdet_save_edg_seq_process );
    REG_PROCESS( vidpro1_save_con_process );
    REG_PROCESS( vidpro1_edgeprune_process );
    REG_PROCESS( vidpro1_smoothcem_process );
    REG_PROCESS( vidpro1_kl_process );
    REG_PROCESS( superimpose_frames_process );
    REG_PROCESS( dbil_osl_canny_subpixel_edges_process); 
    REG_PROCESS( dbrl_edge_point_tangent_bg_model_process); 
    REG_PROCESS( dbrl_edge_point_tangent_fg_detect_process);
    REG_PROCESS( dbrl_denoise_point_set_process);
    //REG_PROCESS( dbrl_load_matchset_process);
    REG_PROCESS( dbrl_crop_id_points_process);
    REG_PROCESS( dbdet_generic_edge_detector_process);
    REG_PROCESS( dbdet_third_order_edge_detector_process);
    REG_PROCESS( dbdet_third_order_color_edge_detector_process);
    REG_PROCESS( dbdet_sel_process );
    REG_PROCESS( dbrl_oriented_edgemap_to_edge_image_process );
    REG_PROCESS( dbrl_edge_image_to_oriented_edgemap_process );

    REG_PROCESS( dbbgm_aerial_bg_model_process );
    REG_PROCESS( dbbgm_aerial_bg_model_process1 );
    REG_PROCESS( dbbgm_aerial_fg_uncertainity_detect_process);
    REG_PROCESS( dbbgm_aerial_fg_uncertainity_detect_process1);
    //REG_PROCESS( dbbgm_bgmodel_edges_process);
    //REG_PROCESS( dbbgm_fgdetect_edges_process);
    REG_PROCESS( dbdet_blob_finder_process );
    REG_PROCESS( dbbgm_load_bg_model_process );
    REG_PROCESS( dbbgm_load_bg_model_process1 );
    REG_PROCESS( dbbgm_save_bg_model_process );
    REG_PROCESS( dbbgm_save_bg_model_process1 );
    REG_PROCESS( dbsol_scan_polygon_process );
    REG_PROCESS(dbru_load_polygons_process);
    REG_PROCESS( dbinfo_multi_track_process );
    REG_PROCESS( dbinfo_multi_poly_track_process );
    REG_PROCESS( dbinfo_load_tracks_process );
    REG_PROCESS( dbinfo_save_tracks_process );
    REG_PROCESS( dbinfo_load_vj_polys_process );
    //REG_PROCESS( dbrl_superresolution_multiple_objects );
    REG_PROCESS( dbrl_postprocess_observation );
    REG_PROCESS( dbrl_superimpose_2ndtier );
    //REG_PROCESS( dbrl_compute_motion_field_process);
    REG_PROCESS( dbacm_geodesic_active_contour_process );
    //REG_PROCESS( dbrl_load_vsol_process );

    // --- other processes ---
    REG_PROCESS( edgemodel_image_to_vsol_process);
    REG_PROCESS( dbrl_rpm_rigid_process);
    REG_PROCESS( dbrl_rpm_affine_process);
    REG_PROCESS( dbrl_rpm_tps_process);
    REG_PROCESS( dbrl_ground_truth_point_set_generator);
    REG_PROCESS( dbrl_rpm_tps_superimpose_process );
    REG_PROCESS( dbrl_rpm_tps_affine_superimpose_process );
    REG_PROCESS( dbrl_rpm_affine_superimpose_process );
    //REG_PROCESS( vsol_to_image_process );
    REG_PROCESS( dbrl_rpm_tps_affine_superimpose_fgbg_process );
    REG_PROCESS( dbrl_rpm_tps_affine_local_global_process );
    REG_PROCESS( dbrl_compare_matchsets_process);
    REG_PROCESS( dbrl_load_multiple_instance_object_process);
    //REG_PROCESS( dbrl_warp_image_process );
    //REG_PROCESS( dbrl_support_process );
    REG_PROCESS( dbrl_support_subpixel_process);
    //REG_PROCESS( dbrl_compute_amir_curvelets_process);
    REG_PROCESS( dbrl_load_con_process);
    REG_PROCESS( dbil_wshed2d_process);
    REG_PROCESS( vidpro1_harris_corners_process);
    REG_PROCESS( dbdet_generic_linker_process);

    georegister_gui_menu menubar;
    vgui_menu menu_holder = menubar.setup_menu(); 
    unsigned w = 500, h = 500;
    vcl_string title = "Point Registration";
    vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
    win->get_adaptor()->set_tableau( bvis1_manager::instance() );

   // vidpro1_process_manager::set_repository( bvis1_manager::instance());
    win->show();
    return vgui::run(); 
}

