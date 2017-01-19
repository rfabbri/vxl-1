# This is brcv/rec/dbskr/algo/CMakeLists.txt
# The Development Brown Shock Recognition Library - Specific Algorithms

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/shp)
include_directories( ${lems_SOURCE_DIR}/brcv/rec)
include_directories( ${lems_SOURCE_DIR}/brcv/seg)
include_directories( ${lems_SOURCE_DIR}/breye1 )

set( dbskr_algo_sources
   dbskr_rec_algs.h dbskr_rec_algs.cxx
   dbskr_shock_patch_selector.h dbskr_shock_patch_selector.cxx
   dbskr_shock_patch_model_selector.h dbskr_shock_patch_model_selector.cxx
   dbskr_shock_patch_match.h dbskr_shock_patch_match.cxx
   dbskr_shock_patch_match_sptr.h
   dbskr_shock_patch_curve_match.h dbskr_shock_patch_curve_match.cxx
   dbskr_shock_patch_curve_match_sptr.h
   dbskr_shock_path_finder.h dbskr_shock_path_finder.cxx
   dbskr_shock_path_finder_sptr.h
)

aux_source_directory(Templates dbskr_algo_sources)

add_library( dbskr_algo  ${dbskr_algo_sources} )

target_link_libraries( dbskr_algo dbskr bgrld dbdet_algo bil_algo vul) 

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

add_subdirectory( io )






