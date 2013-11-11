IF(VXL_VGUI_FOUND)
  INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
ENDIF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p)
INCLUDE_DIRECTORIES( ${lems_BIOTREE_INCLUDE_DIR}/biosim)

IF( BUILD_CONTRIB_BIOTREE_biov )

ADD_EXECUTABLE(vol3d_gaussian_filtering_example vol3d_gaussian_filtering_example.cxx )
TARGET_LINK_LIBRARIES(vol3d_gaussian_filtering_example vbl vcl vol3d vol3d_algo vol3d_io vil3d vil bioproc bioproc_io)

ADD_EXECUTABLE(vol3d_edge_detection_example vol3d_edge_detection_example.cxx )
TARGET_LINK_LIBRARIES(vol3d_edge_detection_example vbl vcl vol3d vol3d_algo bioproc bioproc_io biodet)

ADD_EXECUTABLE(vol3d_radius_detection_example vol3d_radius_detection_example.cxx)
TARGET_LINK_LIBRARIES(vol3d_radius_detection_example vbl vcl vol3d vol3d_io vol3d_algo vgui bgui3d biov biosim)

ADD_EXECUTABLE(vol3d_real_data_radius_detection_example vol3d_real_data_radius_detection_example.cxx )
TARGET_LINK_LIBRARIES(vol3d_real_data_radius_detection_example vbl vcl vol3d vol3d_io vol3d_algo vgui bgui3d biov bsta)

ENDIF( BUILD_CONTRIB_BIOTREE_biov )







