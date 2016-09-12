# This is brcv/mvg/dbvrl/pro/CMakeLists.txt
# Brown Video Registration Processing library

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )      
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
set(dbvrl_pro_sources
   dbvrl_video_process.cxx          dbvrl_video_process.h
   dbvrl_region_process.cxx         dbvrl_region_process.h
   dbvrl_region_storage.cxx         dbvrl_region_storage.h        dbvrl_region_storage_sptr.h
   dbvrl_compute_homography_process.cxx  dbvrl_compute_homography_process.h
   dbvrl_register_images_process.cxx    dbvrl_register_images_process.h
   dbvrl_compute_homography_fast_process.cxx dbvrl_compute_homography_fast_process.h


)

#aux_source_directory(Templates dbvrl_pro_sources)

add_library(dbvrl_pro ${dbvrl_pro_sources})

target_link_libraries(dbvrl_pro dbvrl vidpro1 bsta)


if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)


