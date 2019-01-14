# This is contrib/dbvxm/pro/CMakeLists.txt
# dbvxm's Process Library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas/dbil/algo )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${LEMS_SOURCE_DIR}/contrib/dbvxm )

set(dbvxm_pro_sources
    dbvxm_detect_instance_process.h                                 dbvxm_detect_instance_process.cxx
    dbvxm_detect_scale_process.h                                        dbvxm_detect_scale_process.cxx
    dbvxm_update_multiscale_process.h                           dbvxm_update_multiscale_process.cxx
    dbvxm_create_multiscale_voxel_world_process.h       dbvxm_create_multiscale_voxel_world_process.cxx
    bvxm_save_multiscale_occupancy_raw_process.h bvxm_save_multiscale_occupancy_raw_process.cxx
    dbvxm_detect_multiscale_process.h dbvxm_detect_multiscale_process.cxx
    dbvxm_compare_3d_voxels_process.h dbvxm_compare_3d_voxels_process.cxx
    dbvxm_register_feature_process.h         dbvxm_register_feature_process.cxx
#    dbvxm_thresholding.h         dbvxm_thresholding.cxx
)

aux_source_directory(Templates dbvxm_pro_sources)

add_library(dbvxm_pro ${dbvxm_pro_sources})

target_link_libraries(dbvxm_pro dbvxm  bvxm_io bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil vil_io vil_pro vnl vnl_io bgeo vgl vgl_algo brip brdb)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )




