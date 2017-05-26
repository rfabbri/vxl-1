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

set( dborl_geometric_contour_breaker_sources
  dborl_geometric_contour_breaker_params.h
  dborl_geometric_contour_breaker_params_sptr.h
  vbl_smart_ptr+dborl_geometric_contour_breaker_params.cxx
  dborl_geometric_contour_breaker_main.cxx
)

set( dborl_graphical_model_contour_merge_sources
  dborl_graphical_model_contour_merge_params.h
  dborl_graphical_model_contour_merge_params_sptr.h
  vbl_smart_ptr+dborl_graphical_model_contour_merge_params.cxx
  dborl_graphical_model_contour_merge_main.cxx
)

set( dborl_contour_ranker_sources
  dborl_contour_ranker_params.h
  dborl_contour_ranker_params_sptr.h
  vbl_smart_ptr+dborl_contour_ranker_params.cxx
  dborl_contour_ranker_main.cxx
)

add_executable (dborl_compute_contours_yuliang ${dborl_edge_det_link_sources} )
add_executable (dborl_compute_curvelets_yuliang ${dborl_compute_curvelets_sources} )
add_executable (dborl_geometric_contour_breaker_yuliang ${dborl_geometric_contour_breaker_sources} )
add_executable (dborl_graphical_model_contour_merge_yuliang ${dborl_graphical_model_contour_merge_sources} )
add_executable (dborl_contour_ranker_yuliang ${dborl_contour_ranker_sources} )
target_link_libraries (dborl_compute_contours_yuliang dborl_algo dbdet_pro)
target_link_libraries (dborl_compute_curvelets_yuliang dborl_algo dbdet_pro)
target_link_libraries (dborl_geometric_contour_breaker_yuliang dborl_algo dbdet_pro)
target_link_libraries (dborl_graphical_model_contour_merge_yuliang dborl_algo dbdet_pro)
target_link_libraries (dborl_contour_ranker_yuliang dborl_algo dbdet_pro)
