# This is dborl/algo/vox_detect_xgraph2/CMakeLists.txt

#TARGET_LINK_LIBRARIES (vox_detect_xgraph2_lib dborl_algo dbsks dbsks_pro dbsksp_xio vidpro1)

# Main executable for vox
SET( vox_detect_xgraph2_sources
  vox_detect_xgraph_using_pyramid_edgemap.h
  vox_detect_xgraph_using_pyramid_edgemap.cxx
  vox_detect_xgraph2_params.h 
  vox_detect_xgraph2_params_sptr.h
  vox_detect_xgraph2_params.cxx
  vbl_smart_ptr+vox_detect_xgraph2_params-.cxx
  vox_detect_xgraph2_main.cxx
)
ADD_EXECUTABLE (vox_detect_xgraph2 ${vox_detect_xgraph2_sources} )
TARGET_LINK_LIBRARIES (vox_detect_xgraph2 dborl_algo dbsks_xio dbsks_algo dbsks_pro dbsksp_xio)



