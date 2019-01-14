

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/contrib/Gregistration )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/basic )
set( georegistration_pro_sources
        dbrl_rpm_rigid_process.h        dbrl_rpm_rigid_process.cxx
        dbrl_rpm_affine_process.h       dbrl_rpm_affine_process.cxx
        dbrl_rpm_tps_process.h  dbrl_rpm_tps_process.cxx
        dbrl_match_set_storage.h dbrl_match_set_storage.cxx dbrl_match_set_storage_sptr.h
        dbrl_id_point_2d_storage.h dbrl_id_point_2d_storage.cxx dbrl_id_point_2d_storage_sptr.h
        dbrl_ground_truth_point_set_generator.h dbrl_ground_truth_point_set_generator.cxx
    edgemodel_image_to_vsol_process.cxx                      edgemodel_image_to_vsol_process.h
    dbrl_rpm_tps_superimpose_process.h dbrl_rpm_tps_superimpose_process.cxx
    dbrl_rpm_tps_affine_superimpose_process.h dbrl_rpm_tps_affine_superimpose_process.cxx
    dbrl_rpm_affine_superimpose_process.h dbrl_rpm_affine_superimpose_process.cxx

    dbrl_rpm_tps_affine_superimpose_fgbg_process.h dbrl_rpm_tps_affine_superimpose_fgbg_process.cxx
    #dbrl_warp_image_process.h          dbrl_warp_image_process.cxx
    superimpose_frames_process.h superimpose_frames_process.cxx
    #dbrl_support_process.h      dbrl_support_process.cxx
    dbrl_support_subpixel_process.h     dbrl_support_subpixel_process.cxx
    dbil_osl_canny_subpixel_edges_process.h                     dbil_osl_canny_subpixel_edges_process.cxx
    dbrl_edge_point_tangent_bg_model_process.h dbrl_edge_point_tangent_bg_model_process.cxx
    dbrl_edge_point_tangent_fg_detect_process.h dbrl_edge_point_tangent_fg_detect_process.cxx
    dbrl_denoise_point_set_process.h dbrl_denoise_point_set_process.cxx
    dbrl_crop_id_points_process.h       dbrl_crop_id_points_process.cxx
    dbrl_local_global_match_set_storage.h dbrl_local_global_match_set_storage_sptr.h
    dbrl_rpm_tps_affine_local_global_process.h  dbrl_rpm_tps_affine_local_global_process.cxx
    dbrl_compare_matchsets_process.h                    dbrl_compare_matchsets_process.cxx
    #dbrl_superresolution_multiple_objects.h dbrl_superresolution_multiple_objects.cxx
    dbrl_postprocess_observation.h dbrl_postprocess_observation.cxx
    dbrl_superimpose_2ndtier.h dbrl_superimpose_2ndtier.cxx
    #vsol_to_image_process.h vsol_to_image_process.cxx
    #dbrl_load_vsol_process.h    dbrl_load_vsol_process.cxx
    #dbrl_save_vsol_process.h    dbrl_save_vsol_process.cxx
    #dbrl_load_matchset_process.cxx dbrl_load_matchset_process.h
    #dbrl_compute_amir_curvelets_process.cxx dbrl_compute_amir_curvelets_process.h
    dbrl_load_con_process.h  dbrl_load_con_process.cxx
    #dbbgm_bgmodel_edges_process.h dbbgm_bgmodel_edges_process.cxx
    #dbbgm_fgdetect_edges_process.h dbbgm_fgdetect_edges_process.cxx
    dbrl_load_multiple_instance_object_process.h dbrl_load_multiple_instance_object_process.cxx
    dbrl_edge_image_to_oriented_edgemap_process.h dbrl_edge_image_to_oriented_edgemap_process.cxx
    dbrl_oriented_edgemap_to_edge_image_process.h dbrl_oriented_edgemap_to_edge_image_process.cxx
 )


aux_source_directory(Templates georegistration_pro_sources)
add_library( georegistration_pro  ${georegistration_pro_sources})

target_link_libraries( georegistration_pro georegister bpro1  vbl vil_io bpro1 osl dbbgm_pro dbsta_algo dbbgm sdet)
