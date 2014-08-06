SET( dborl_edge_det_link_sources
  dborl_edge_det_link_main.cxx 
)

ADD_EXECUTABLE (dborl_compute_curve_frags ${dborl_edge_det_link_sources} )
TARGET_LINK_LIBRARIES (dborl_compute_curve_frags dborl_algo dbdet_pro)
