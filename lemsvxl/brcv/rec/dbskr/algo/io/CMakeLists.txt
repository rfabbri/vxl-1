# This is brcv/rec/dbskr/algo/io/CMakeLists.txt
# The Development Brown Shock Recognition Library - Specific Algorithms IO

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp)
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec)
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg)
include_directories( ${LEMS_SOURCE_DIR}/basic )

set( dbskr_algo_io_sources
   dbskr_extract_patch_params.h dbskr_extract_patch_params.cxx
   dbskr_match_shock_params.h dbskr_match_shock_params.cxx
   dbskr_match_patch_params.h dbskr_match_patch_params.cxx
   dbskr_detect_patch_params.h dbskr_detect_patch_params.cxx
   dbskr_extract_bnd_params.h dbskr_extract_bnd_params.cxx
   dbskr_extract_shock_params.h dbskr_extract_shock_params.cxx
)

aux_source_directory(Templates dbskr_algo_io_sources)

add_library( dbskr_algoio  ${dbskr_algo_io_sources} )

target_link_libraries( dbskr_algoio dbskr_algo dborl bxml) 

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)






