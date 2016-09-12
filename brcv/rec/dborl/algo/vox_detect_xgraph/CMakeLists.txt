# This is dborl/algo/vox_detect_xgraph/CMakeLists.txt

# Support library
set( vox_detect_xgraph_lib_sources
  vox_detect_xgraph_params.h 
  vox_detect_xgraph_params_sptr.h
  vox_detect_xgraph_params.cxx
  vbl_smart_ptr+vox_detect_xgraph_params-.cxx
)

add_library (vox_detect_xgraph_lib ${vox_detect_xgraph_lib_sources} )
target_link_libraries (vox_detect_xgraph_lib dborl_algo dbsks dbsks_pro dbsksp_xio vidpro1)


# Main executable for vox
set( vox_detect_xgraph_sources
  vox_detect_xgraph_main.cxx
)
add_executable (vox_detect_xgraph ${vox_detect_xgraph_sources} )
target_link_libraries (vox_detect_xgraph vox_detect_xgraph_lib dborl_algo dbsks_xio dbsks_algo dbsks_pro dbsksp_xio)


# Post-processing executable
#set( vox_detect_xgraph_postprocess_sources
#  vox_detect_xgraph_postprocess_main.cxx
#  vox_detect_xgraph_postprocess_utils.h
#  vox_detect_xgraph_postprocess_utils.cxx
#)

#add_executable (vox_detect_xgraph_postprocess ${vox_detect_xgraph_postprocess_sources} )
#target_link_libraries (vox_detect_xgraph_postprocess vox_detect_xgraph_lib dbul dborl_algo dbsks_algo dbsks_xio dbsks_pro dbsksp_xio vidpro1)




