# This is contrib/bvam/pro/CMakeLists.txt
# Bvam's Process Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/dbil/algo )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic/dbil/algo )

SET(bvam_pro_sources
#   bvam_load_voxel_world_process.h       bvam_load_voxel_world_process.cxx
    bvam_create_voxel_world_process.h     bvam_create_voxel_world_process.cxx
    bvam_detect_changes_process.h         bvam_detect_changes_process.cxx
    bvam_generate_edge_map_process.h      bvam_generate_edge_map_process.cxx
    bvam_render_virtual_view_process.h    bvam_render_virtual_view_process.cxx
    bvam_render_expected_image_process.h  bvam_render_expected_image_process.cxx
    bvam_change_detection_display_process.h bvam_change_detection_display_process.cxx
    bvam_update_process.h                 bvam_update_process.cxx
    bvam_normalize_image_process.h        bvam_normalize_image_process.cxx
    bvam_illum_index_process.h            bvam_illum_index_process.cxx
    bvam_roi_init_process.h               bvam_roi_init_process.cxx
    bvam_rpc_registration_process.h       bvam_rpc_registration_process.cxx
    bvam_save_occupancy_raw_process.h     bvam_save_occupancy_raw_process.cxx
    bvam_clean_world_process.h            bvam_clean_world_process.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvam_pro_sources)

ADD_LIBRARY(bvam_pro ${bvam_pro_sources})

TARGET_LINK_LIBRARIES(bvam_pro bvam bvam_io bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vsol vil vil_io vnl vnl_io bgeo vgl vgl_algo brip brdb)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )





