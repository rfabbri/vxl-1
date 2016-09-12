set( dborl_edge_det_link_sources
  dborl_edge_det_link_main.cxx 
)

add_executable (dborl_compute_curve_frags ${dborl_edge_det_link_sources} )
target_link_libraries (dborl_compute_curve_frags dborl_algo dbdet_pro)
