set( dborl_edge_det_link_sources
  dborl_edge_det_link_params.h 
  dborl_edge_det_link_params_sptr.h
  dborl_edge_det_link_params.cxx
  vbl_smart_ptr+dborl_edge_det_link_params-.cxx
  dborl_edge_det_link_main.cxx 
)

set( dborl_compute_curvelets_sources
  dborl_edge_det_link_params.h 
  dborl_edge_det_link_params_sptr.h
  dborl_edge_det_link_params.cxx
  vbl_smart_ptr+dborl_edge_det_link_params-.cxx
  dborl_compute_curvelets_main.cxx 
)

add_executable (dborl_compute_contours_yuliang ${dborl_edge_det_link_sources} )
add_executable (dborl_compute_curvelets_yuliang ${dborl_compute_curvelets_sources} )
target_link_libraries (dborl_compute_contours_yuliang dborl_algo dbdet_pro)
target_link_libraries (dborl_compute_curvelets_yuliang dborl_algo dbdet_pro)
