# mesh_vis application
# contrib/meshvis/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#INCLUDE( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET(meshvis_sources

   meshvis_main.cxx

)


ADD_EXECUTABLE( meshvis ${meshvis_sources} )

TARGET_LINK_LIBRARIES( meshvis dbmsh3d dbmsh3d_algo dbmsh3d_vis dbul bgui3d vsol vpgl vsl)

ADD_EXECUTABLE(mesh_project_real mesh_project_real.cxx)
TARGET_LINK_LIBRARIES( mesh_project_real dbmsh3d dbmsh3d_vis bgui3d )

ENDIF (HAS_BGUI3D)

