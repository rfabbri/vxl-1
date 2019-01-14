# mesh_vis application
# contrib/meshvis/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#include( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

if (HAS_BGUI3D)

set(meshvis_sources

   meshvis_main.cxx

)


add_executable( meshvis ${meshvis_sources} )

target_link_libraries( meshvis dbmsh3d dbmsh3d_algo dbmsh3d_vis dbul bgui3d vsol vpgl vsl)

add_executable(mesh_project_real mesh_project_real.cxx)
target_link_libraries( mesh_project_real dbmsh3d dbmsh3d_vis bgui3d )

endif (HAS_BGUI3D)

