# This is contrib/dbvxm/pro/CMakeLists.txt
# dbvxm's Process Library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas/dbil/algo )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${lems_SOURCE_DIR}/contrib/dbvxm )

set(dbvxm_rec_pro_sources
    dbvxm_rec_update_changes_process.h  dbvxm_rec_update_changes_process.cxx
    dbvxm_rec_update_changes_using_mog_process.h  dbvxm_rec_update_changes_using_mog_process.cxx
    dbvxm_rec_density_to_prob_map_process.h  dbvxm_rec_density_to_prob_map_process.cxx
    dbvxm_prob_map_roc_compute_process.h         dbvxm_prob_map_roc_compute_process.cxx
    dbvxm_prob_map_threshold_process.h         dbvxm_prob_map_threshold_process.cxx
    dbvxm_rec_structure_process.h  dbvxm_rec_structure_process.cxx
    dbvxm_prob_map_supress_process.h         dbvxm_prob_map_supress_process.cxx
    dbvxm_prob_map_area_process.h         dbvxm_prob_map_area_process.cxx
)

#aux_source_directory(Templates dbvxm_rec_pro_sources)

add_library(dbvxm_rec_pro ${dbvxm_rec_pro_sources})

target_link_libraries(dbvxm_rec_pro dbvxm_rec dbvxm bvxm_io bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil vil_io vil_pro vnl vnl_io bgeo vgl vgl_algo brip brdb)

if( BUILD_TESTING )
  subdirs(tests)
endif( BUILD_TESTING )




