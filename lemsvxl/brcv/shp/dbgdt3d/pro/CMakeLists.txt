# This is contrib/dbmsh3d/dbgdt3d/pro/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )

set ( dbgdt3d_vis_sources
   dbgdt3d_process.cxx              dbgdt3d_process.h
   dbgdt3d_cmdpara.cxx              dbgdt3d_cmdpara.h

)

add_library( dbgdt3d_pro ${dbgdt3d_vis_sources})

target_link_libraries( dbgdt3d_pro dbmsh3d_pro dbmsh3d dbgdt3d )

