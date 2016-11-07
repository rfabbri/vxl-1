include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${VXL_EXPAT_INCLUDE_DIR} )
include_directories(${VXL_BRL_INCLUDE_DIR}/b3p )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set(dborl_pb_edge_det_sources
  dborl_pb_edge_det_params.h
  dborl_pb_edge_det_params_sptr.h
  dborl_pb_edge_det_params.cxx
  dborl_pb_edge_det_main.cxx
  vbl_smart_ptr+dborl_pb_edge_det_params-.cxx
)

add_executable (dborl_pb_edge_det ${dborl_pb_edge_det_sources})
target_link_libraries (dborl_pb_edge_det dborl_algo)

add_subdirectory(vox_third_order_edge_det)
add_subdirectory(index_label_test)
