if(VXL_VGUI_FOUND)
  include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
endif (VXL_VGUI_FOUND)

include_directories(${LEMS_SOURCE_DIR}/basic ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_GEL_INCLUDE_DIR})
include_directories( ${LEMS_SOURCE_DIR}/db3p)

if( BUILD_CONTRIB_BIOTREE_biov )

add_executable(vol3d_synthetic_cyl_generator vol3d_synthetic_cyl_generator.cxx )
target_link_libraries(vol3d_synthetic_cyl_generator vbl vcl vol3d)

add_executable(vol3d_deformed_torus_generator vol3d_deformed_torus_generator.cxx )
target_link_libraries(vol3d_deformed_torus_generator vbl vcl vol3d biosim)

endif( BUILD_CONTRIB_BIOTREE_biov )


