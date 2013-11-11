# This is contrib/dbmsh3d/dbgdt3d/cmdproc/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET ( dbgdt3d_cmdproc_sources
   dbgdt3d_process_vis.cxx          dbgdt3d_process_vis.h
   dbgdt3d_cmdproc.cxx              dbgdt3d_cmdproc.h

)

ADD_LIBRARY( dbgdt3d_cmdproc ${dbgdt3d_cmdproc_sources})

TARGET_LINK_LIBRARIES( dbgdt3d_cmdproc dbmsh3d dbmsh3d_pro 
                       dbgdt3d dbgdt3d_pro dbgdt3d_vis )

ENDIF (HAS_BGUI3D)

