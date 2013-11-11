SET( dborl_edge_det_link_sources
  dborl_edge_det_link_params.h 
  dborl_edge_det_link_params_sptr.h
  dborl_edge_det_link_params.cxx
  vbl_smart_ptr+dborl_edge_det_link_params-.cxx
  dborl_edge_det_link_main.cxx 
)

SET( dborl_compute_curvelets_sources
  dborl_edge_det_link_params.h 
  dborl_edge_det_link_params_sptr.h
  dborl_edge_det_link_params.cxx
  vbl_smart_ptr+dborl_edge_det_link_params-.cxx
  dborl_compute_curvelets_main.cxx 
)

ADD_EXECUTABLE (dborl_compute_contours_yuliang ${dborl_edge_det_link_sources} )
ADD_EXECUTABLE (dborl_compute_curvelets_yuliang ${dborl_compute_curvelets_sources} )
TARGET_LINK_LIBRARIES (dborl_compute_contours_yuliang dborl_algo dbdet_pro)
TARGET_LINK_LIBRARIES (dborl_compute_curvelets_yuliang dborl_algo dbdet_pro)
