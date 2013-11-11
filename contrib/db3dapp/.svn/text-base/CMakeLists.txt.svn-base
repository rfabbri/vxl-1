# 3D application
# contrib/db3dapp/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#INCLUDE( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET(db3d_app_sources

   main.cxx
   MeshEditorMenu.h      MeshEditorMenu.cxx
   MenuHandler.h         MenuHandler.cxx
   SceneHandler.h        SceneHandler.cxx
   SceneEventHandler.h   SceneEventHandler.cxx

)


ADD_EXECUTABLE(db3d_app ${db3d_app_sources})

TARGET_LINK_LIBRARIES( db3d_app bgui3d dbsk3d dbsk3d_algo dbmsh dbmsh_vis )

ENDIF (HAS_BGUI3D)

# Brown Mesh Library
SUBDIRS( dbmsh )


