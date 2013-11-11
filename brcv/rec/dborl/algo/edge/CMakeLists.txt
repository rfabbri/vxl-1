INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

SET( dborl_third_order_edge_det_sources
  dborl_third_order_edge_det_params.h 
  dborl_third_order_edge_det_params_sptr.h
  dborl_third_order_edge_det_main.cxx
  vbl_smart_ptr+dborl_third_order_edge_det_params-.cxx
)

ADD_EXECUTABLE (dborl_edge_third_order ${dborl_third_order_edge_det_sources})
TARGET_LINK_LIBRARIES (dborl_edge_third_order dborl_algo dbdet_pro)



