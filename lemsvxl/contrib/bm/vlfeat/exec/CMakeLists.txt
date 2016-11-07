include_directories( ${VL_FEAT_INCLUDE_PATH} )
add_executable( vl_feat_test vl_feat_test.cxx )
target_link_libraries( vl_feat_test ${VL_FEAT_LIBRARY} )
