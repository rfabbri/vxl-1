# This is dbsks/algo/CMakeLists.txt
# The Algorithm Library for Shock-Based Segmentation
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )

set( dbsks_detect_xgraph_course_to_fine_sources  
  dbsks_detect_xgraph_course_to_fine.h   dbsks_detect_xgraph_course_to_fine.cxx  
  
)
add_library( dbsks_detect_xgraph_course_to_fine  ${dbsks_detect_xgraph_course_to_fine_sources} )
target_link_libraries( dbsks_detect_xgraph_course_to_fine dbsks dbsks_xio dbul dborl )

set(vox_detect_xgraph_course_to_fine_sources
vox_detect_xgraph_course_to_fine_main.cxx vox_detect_xgraph_params.h
vox_detect_xgraph_params.cxx vox_detect_xgraph_params_sptr.h
vbl_smart_ptr+vox_detect_xgraph_params-.cxx
)
add_executable (vox_detect_xgraph_course_to_fine ${vox_detect_xgraph_course_to_fine_sources} )
target_link_libraries (vox_detect_xgraph_course_to_fine dbsks_detect_xgraph_course_to_fine dborl_algo dbsks_xio dbsks_algo dbsks_pro dbsksp_xio)




