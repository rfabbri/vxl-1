# This is brcv/mvg/dbvrl/pro/CMakeLists.txt
# Brown Video Registration Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )      
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
SET(dbvrl_pro_sources
   dbvrl_video_process.cxx          dbvrl_video_process.h
   dbvrl_region_process.cxx         dbvrl_region_process.h
   dbvrl_region_storage.cxx         dbvrl_region_storage.h        dbvrl_region_storage_sptr.h
   dbvrl_compute_homography_process.cxx  dbvrl_compute_homography_process.h
   dbvrl_register_images_process.cxx    dbvrl_register_images_process.h
   dbvrl_compute_homography_fast_process.cxx dbvrl_compute_homography_fast_process.h


)

#AUX_SOURCE_DIRECTORY(Templates dbvrl_pro_sources)

ADD_LIBRARY(dbvrl_pro ${dbvrl_pro_sources})

TARGET_LINK_LIBRARIES(dbvrl_pro dbvrl vidpro1 bsta)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)


