INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/seg/)

SET( vox_third_order_edge_det_sources
  vox_third_order_edge_det_params.h 
  vox_third_order_edge_det_params_sptr.h
  vox_third_order_edge_det_params.cxx
  vbl_smart_ptr+vox_third_order_edge_det_params-.cxx
  vox_third_order_edge_det_main.cxx 
)

ADD_EXECUTABLE (vox_third_order_edge_det ${vox_third_order_edge_det_sources} )
TARGET_LINK_LIBRARIES (vox_third_order_edge_det dborl_algo dbdet_pro)



