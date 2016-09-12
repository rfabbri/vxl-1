if(VXL_VGUI_FOUND)
  include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
endif (VXL_VGUI_FOUND)

include_directories(${lems_SOURCE_DIR}/basic ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_GEL_INCLUDE_DIR})
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${lems_SOURCE_DIR}/db3p)
include_directories( ${lems_BIOTREE_INCLUDE_DIR}/biosim)

if( BUILD_CONTRIB_BIOTREE_biov )

add_executable(vol3d_gaussian_filtering_example vol3d_gaussian_filtering_example.cxx )
target_link_libraries(vol3d_gaussian_filtering_example vbl vcl vol3d vol3d_algo vol3d_io vil3d vil bioproc bioproc_io)

add_executable(vol3d_edge_detection_example vol3d_edge_detection_example.cxx )
target_link_libraries(vol3d_edge_detection_example vbl vcl vol3d vol3d_algo bioproc bioproc_io biodet)

add_executable(vol3d_radius_detection_example vol3d_radius_detection_example.cxx)
target_link_libraries(vol3d_radius_detection_example vbl vcl vol3d vol3d_io vol3d_algo vgui bgui3d biov biosim)

add_executable(vol3d_real_data_radius_detection_example vol3d_real_data_radius_detection_example.cxx )
target_link_libraries(vol3d_real_data_radius_detection_example vbl vcl vol3d vol3d_io vol3d_algo vgui bgui3d biov bsta)

endif( BUILD_CONTRIB_BIOTREE_biov )







