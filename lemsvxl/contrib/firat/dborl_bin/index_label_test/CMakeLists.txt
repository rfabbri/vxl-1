include_directories(${LEMS_SOURCE_DIR}/brcv/seg/)

set( vox_test_index_label_sources
  vox_test_index_label_params.h 
  vox_test_index_label_params_sptr.h
  vox_test_index_label_params.cxx
  vbl_smart_ptr+vox_test_index_label_params-.cxx
  vox_test_index_label_main.cxx 
)

add_executable (vox_test_index_label ${vox_test_index_label_sources} )
target_link_libraries (vox_test_index_label dborl_algo)





