if(SIMVOLEON_FOUND)
  include_directories(${LEMS_SOURCE_DIR}/basic ${LEMS_SOURCE_DIR}/contrib/biotree/proc ${VXL_GEL_INCLUDE_DIR})
  include_directories( ${LEMS_SOURCE_DIR}/db3p )
  include_directories(${LEMS_SOURCE_DIR}/contrib/biotree/resc ${LEMS_SOURCE_DIR}/contrib/biotree/biosim)
  include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
  include_directories( ${VXL_MUL_INCLUDE_DIR} )

  add_executable( det_hair_nonmax_sup_example det_hair_nonmax_sup_example.cxx)
  add_executable( det_edge_nonmax_supp_example det_edge_nonmax_supp_example.cxx)
  add_executable( det_edge_nonmax_supp_example_txt_version det_edge_nonmax_supp_example_txt_version.cxx)
  add_executable( det_hair_nonmax_sup_from_response_example det_hair_nonmax_sup_from_response_example.cxx)  
#  add_executable( det_detect_cylinder_example det_detect_cylinder_example.cxx)
  add_executable( det_edge_nonmax_supp_example_from_gradient det_edge_nonmax_supp_example_from_gradient.cxx)

  target_link_libraries(det_hair_nonmax_sup_example vgui bgui3d biob biov biodet )
  target_link_libraries(det_hair_nonmax_sup_from_response_example bioproc_io vgl vsol biob biov biodet )
  target_link_libraries(det_edge_nonmax_supp_example bioproc_io vgui vgl vsol bgui3d biov biodet )
  target_link_libraries(det_edge_nonmax_supp_example_txt_version bioproc_io vgui vgl vsol bgui3d biov biodet )
#  target_link_libraries(det_detect_cylinder_example bioproc_io vgui bgui3d biov biodet bioproc_util )  
  target_link_libraries(det_edge_nonmax_supp_example_from_gradient bioproc_io vgui bgui3d biov biov biob biodet bioproc_util )  

#  target_link_libraries(det_cylinder_splr vgui vgl vsol bgui3d biob biov biodet bioproc_io geom)
  
#  add_executable( det_radius_vol3d_dection_example det_radius_vol3d_detection_example.cxx)
#  target_link_libraries(det_radius_vol3d_dection_example vol3d vol3d_algo vgui bgui3d biov biodet biosim)

endif(SIMVOLEON_FOUND)

