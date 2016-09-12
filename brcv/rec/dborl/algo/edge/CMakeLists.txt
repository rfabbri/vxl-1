include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

set( dborl_third_order_edge_det_sources
  dborl_third_order_edge_det_params.h 
  dborl_third_order_edge_det_params_sptr.h
  dborl_third_order_edge_det_main.cxx
  vbl_smart_ptr+dborl_third_order_edge_det_params-.cxx
)

add_executable (dborl_edge_third_order ${dborl_third_order_edge_det_sources})
target_link_libraries (dborl_edge_third_order dborl_algo dbdet_pro)



