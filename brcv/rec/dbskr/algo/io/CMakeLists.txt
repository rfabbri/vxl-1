# This is brcv/rec/dbskr/algo/io/CMakeLists.txt
# The Development Brown Shock Recognition Library - Specific Algorithms IO

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SET( dbskr_algo_io_sources
   dbskr_extract_patch_params.h dbskr_extract_patch_params.cxx
   dbskr_match_shock_params.h dbskr_match_shock_params.cxx
   dbskr_match_patch_params.h dbskr_match_patch_params.cxx
   dbskr_detect_patch_params.h dbskr_detect_patch_params.cxx
   dbskr_extract_bnd_params.h dbskr_extract_bnd_params.cxx
   dbskr_extract_shock_params.h dbskr_extract_shock_params.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbskr_algo_io_sources)

ADD_LIBRARY( dbskr_algoio  ${dbskr_algo_io_sources} )

TARGET_LINK_LIBRARIES( dbskr_algoio dbskr_algo dborl bxml) 

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)






