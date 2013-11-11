# This is contrib/dbmsh3d/dbgdt3d/pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

SET ( dbgdt3d_vis_sources
   dbgdt3d_process.cxx              dbgdt3d_process.h
   dbgdt3d_cmdpara.cxx              dbgdt3d_cmdpara.h

)

ADD_LIBRARY( dbgdt3d_pro ${dbgdt3d_vis_sources})

TARGET_LINK_LIBRARIES( dbgdt3d_pro dbmsh3d_pro dbmsh3d dbgdt3d )

