# This is contrib/dbvxm/pro/CMakeLists.txt
# dbvxm's Process Library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas/dbil/algo )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbvxm )

SET(dbvxm_pro_sources
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

AUX_SOURCE_DIRECTORY(Templates dbvxm_pro_sources)

ADD_LIBRARY(dbvxm_pro ${dbvxm_pro_sources})

TARGET_LINK_LIBRARIES(dbvxm_pro dbvxm  bvxm_io bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil vil_io vil_pro vnl vnl_io bgeo vgl vgl_algo brip brdb)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )




