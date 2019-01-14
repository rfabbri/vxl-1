# This is contrib/dbmsh3d/gdt/pro/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
if (HAS_BGUI3D)

set ( gdt_vis_sources
   gdt_process.cxx              gdt_process.h
   gdt_cmdpara.cxx              gdt_cmdpara.h

)

add_library( gdt_pro ${gdt_vis_sources})

target_link_libraries( gdt_pro dbmsh3d_pro dbmsh3d gdt )

endif (HAS_BGUI3D)

