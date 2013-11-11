IF(VXL_VGUI_FOUND)
  INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
ENDIF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p)

IF( BUILD_CONTRIB_BIOTREE_biov )

ADD_EXECUTABLE(vol3d_synthetic_cyl_generator vol3d_synthetic_cyl_generator.cxx )
TARGET_LINK_LIBRARIES(vol3d_synthetic_cyl_generator vbl vcl vol3d)

ADD_EXECUTABLE(vol3d_deformed_torus_generator vol3d_deformed_torus_generator.cxx )
TARGET_LINK_LIBRARIES(vol3d_deformed_torus_generator vbl vcl vol3d biosim)

ENDIF( BUILD_CONTRIB_BIOTREE_biov )


