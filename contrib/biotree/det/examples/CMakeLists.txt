IF(SIMVOLEON_FOUND)
  INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic ${lems_SOURCE_DIR}/contrib/biotree/proc ${VXL_GEL_INCLUDE_DIR})
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
  INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib/biotree/resc ${lems_SOURCE_DIR}/contrib/biotree/biosim)
  INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

  ADD_EXECUTABLE( det_hair_nonmax_sup_example det_hair_nonmax_sup_example.cxx)
  ADD_EXECUTABLE( det_edge_nonmax_supp_example det_edge_nonmax_supp_example.cxx)
  ADD_EXECUTABLE( det_edge_nonmax_supp_example_txt_version det_edge_nonmax_supp_example_txt_version.cxx)
  ADD_EXECUTABLE( det_hair_nonmax_sup_from_response_example det_hair_nonmax_sup_from_response_example.cxx)  
#  ADD_EXECUTABLE( det_detect_cylinder_example det_detect_cylinder_example.cxx)
  ADD_EXECUTABLE( det_edge_nonmax_supp_example_from_gradient det_edge_nonmax_supp_example_from_gradient.cxx)

  TARGET_LINK_LIBRARIES(det_hair_nonmax_sup_example vgui bgui3d biob biov biodet )
  TARGET_LINK_LIBRARIES(det_hair_nonmax_sup_from_response_example bioproc_io vgl vsol biob biov biodet )
  TARGET_LINK_LIBRARIES(det_edge_nonmax_supp_example bioproc_io vgui vgl vsol bgui3d biov biodet )
  TARGET_LINK_LIBRARIES(det_edge_nonmax_supp_example_txt_version bioproc_io vgui vgl vsol bgui3d biov biodet )
#  TARGET_LINK_LIBRARIES(det_detect_cylinder_example bioproc_io vgui bgui3d biov biodet bioproc_util )  
  TARGET_LINK_LIBRARIES(det_edge_nonmax_supp_example_from_gradient bioproc_io vgui bgui3d biov biov biob biodet bioproc_util )  

#  TARGET_LINK_LIBRARIES(det_cylinder_splr vgui vgl vsol bgui3d biob biov biodet bioproc_io geom)
  
#  ADD_EXECUTABLE( det_radius_vol3d_dection_example det_radius_vol3d_detection_example.cxx)
#  TARGET_LINK_LIBRARIES(det_radius_vol3d_dection_example vol3d vol3d_algo vgui bgui3d biov biodet biosim)

ENDIF(SIMVOLEON_FOUND)

