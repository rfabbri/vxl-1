# This is dborl/algo/vox_generate_shape/CMakeLists.txt

# Support library
SET( vox_generate_shape_lib_sources
  vox_generate_shape_params.h 
  vox_generate_shape_params.cxx
  vox_generate_shape_params_sptr.h
  vbl_smart_ptr+vox_generate_shape_params-.cxx
  dbsks_shape_generator.h
  dbsks_shape_generator.cxx
)

ADD_LIBRARY (vox_generate_shape_lib ${vox_generate_shape_lib_sources} )
TARGET_LINK_LIBRARIES (vox_generate_shape_lib dborl_algo dbsks dbsks_pro dbsksp_xio vidpro1)


# Main executable for vox
SET( vox_generate_shape_sources
  vox_generate_shape_main.cxx
)
ADD_EXECUTABLE (vox_generate_shape ${vox_generate_shape_sources} )
TARGET_LINK_LIBRARIES (vox_generate_shape vox_generate_shape_lib dborl_algo dbsks dbsks_xio dbsks_algo dbsks_pro dbsksp_xio dbdet_pro dbdet)





