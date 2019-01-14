# This is contrib/dbmsh3d/cmdproc/CMakeLists.txt
# 3D application 

include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

#message ( dbmsh3d_cmdproc )

if (HAS_BGUI3D)

#message ( dbmsh3d_cmdproc HAS_BGUI3D )

set(dbmsh3d_cmdproc_sources

   dbmsh3d_cmdproc.cxx          dbmsh3d_cmdproc.h

   dbmsh3d_process_vis.cxx      dbmsh3d_process_vis.h
)

add_library(dbmsh3d_cmdproc ${dbmsh3d_cmdproc_sources})

target_link_libraries( dbmsh3d_cmdproc dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis bgui3d )

endif (HAS_BGUI3D)

