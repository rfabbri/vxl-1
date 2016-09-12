# This is dborl/algo/vox_generate_shape/CMakeLists.txt

# Support library
set( vox_generate_shape_lib_sources
  vox_generate_shape_params.h 
  vox_generate_shape_params.cxx
  vox_generate_shape_params_sptr.h
  vbl_smart_ptr+vox_generate_shape_params-.cxx
  dbsks_shape_generator.h
  dbsks_shape_generator.cxx
)

add_library (vox_generate_shape_lib ${vox_generate_shape_lib_sources} )
target_link_libraries (vox_generate_shape_lib dborl_algo dbsks dbsks_pro dbsksp_xio vidpro1)


# Main executable for vox
set( vox_generate_shape_sources
  vox_generate_shape_main.cxx
)
add_executable (vox_generate_shape ${vox_generate_shape_sources} )
target_link_libraries (vox_generate_shape vox_generate_shape_lib dborl_algo dbsks dbsks_xio dbsks_algo dbsks_pro dbsksp_xio dbdet_pro dbdet)





