set( MSEL_from_edge_sources
  MSEL_compute_contours_from_edges_main.cxx 
)

set( MSEL_from_img_sources
  MSEL_compute_contours_from_img_main.cxx 
)


add_executable (MSEL_compute_contours_from_edges ${MSEL_from_edge_sources} )
target_link_libraries (MSEL_compute_contours_from_edges dborl_algo dbdet_pro)

add_executable (MSEL_compute_contours_from_img ${MSEL_from_img_sources} )
target_link_libraries (MSEL_compute_contours_from_img dborl_algo dbdet_pro)
