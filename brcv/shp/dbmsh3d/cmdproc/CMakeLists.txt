# This is contrib/dbmsh3d/cmdproc/CMakeLists.txt
# 3D application 

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

#MESSAGE ( dbmsh3d_cmdproc )

IF (HAS_BGUI3D)

#MESSAGE ( dbmsh3d_cmdproc HAS_BGUI3D )

SET(dbmsh3d_cmdproc_sources

   dbmsh3d_cmdproc.cxx          dbmsh3d_cmdproc.h

   dbmsh3d_process_vis.cxx      dbmsh3d_process_vis.h
)

ADD_LIBRARY(dbmsh3d_cmdproc ${dbmsh3d_cmdproc_sources})

TARGET_LINK_LIBRARIES( dbmsh3d_cmdproc dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis bgui3d )

ENDIF (HAS_BGUI3D)

