# This is dbsks/algo/CMakeLists.txt
# The Algorithm Library for Shock-Based Segmentation
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )

SET( dbsks_detect_xgraph_course_to_fine_sources  
  dbsks_detect_xgraph_course_to_fine.h   dbsks_detect_xgraph_course_to_fine.cxx  
  
)
ADD_LIBRARY( dbsks_detect_xgraph_course_to_fine  ${dbsks_detect_xgraph_course_to_fine_sources} )
TARGET_LINK_LIBRARIES( dbsks_detect_xgraph_course_to_fine dbsks dbsks_xio dbul dborl )

SET(vox_detect_xgraph_course_to_fine_sources
vox_detect_xgraph_course_to_fine_main.cxx vox_detect_xgraph_params.h
vox_detect_xgraph_params.cxx vox_detect_xgraph_params_sptr.h
vbl_smart_ptr+vox_detect_xgraph_params-.cxx
)
ADD_EXECUTABLE (vox_detect_xgraph_course_to_fine ${vox_detect_xgraph_course_to_fine_sources} )
TARGET_LINK_LIBRARIES (vox_detect_xgraph_course_to_fine dbsks_detect_xgraph_course_to_fine dborl_algo dbsks_xio dbsks_algo dbsks_pro dbsksp_xio)




