# This is contrib/dbmsh3d/dbgdt3d/cmdproc/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

if (HAS_BGUI3D)

set ( dbgdt3d_cmdproc_sources
   dbgdt3d_process_vis.cxx          dbgdt3d_process_vis.h
   dbgdt3d_cmdproc.cxx              dbgdt3d_cmdproc.h

)

add_library( dbgdt3d_cmdproc ${dbgdt3d_cmdproc_sources})

target_link_libraries( dbgdt3d_cmdproc dbmsh3d dbmsh3d_pro 
                       dbgdt3d dbgdt3d_pro dbgdt3d_vis )

endif (HAS_BGUI3D)

