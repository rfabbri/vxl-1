# 3D application
# contrib/db3dapp/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#include( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

if (HAS_BGUI3D)

set(db3d_app_sources

   main.cxx
   MeshEditorMenu.h      MeshEditorMenu.cxx
   MenuHandler.h         MenuHandler.cxx
   SceneHandler.h        SceneHandler.cxx
   SceneEventHandler.h   SceneEventHandler.cxx

)


add_executable(db3d_app ${db3d_app_sources})

target_link_libraries( db3d_app bgui3d dbsk3d dbsk3d_algo dbmsh dbmsh_vis )

endif (HAS_BGUI3D)

# Brown Mesh Library
add_subdirectory( dbmsh )


