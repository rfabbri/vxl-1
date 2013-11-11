# This is brcv/rec/dbskr/algo/CMakeLists.txt
# The Development Brown Shock Recognition Library - Specific Algorithms

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET( dbskr_algo_sources
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

AUX_SOURCE_DIRECTORY(Templates dbskr_algo_sources)

ADD_LIBRARY( dbskr_algo  ${dbskr_algo_sources} )

TARGET_LINK_LIBRARIES( dbskr_algo dbskr dbgrl dbdet_algo bil_algo vul) 

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

SUBDIRS( io )






