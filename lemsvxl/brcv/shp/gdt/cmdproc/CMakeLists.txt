# This is contrib/dbmsh3d/gdt/pro/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
if (HAS_BGUI3D)

set ( gdt_vis_sources
   gdt_cmdproc.cxx              gdt_cmdproc.h

)

add_library( gdt_cmdproc ${gdt_vis_sources})

target_link_libraries( gdt_cmdproc dbmsh3d dbmsh3d_pro gdt gdt_pro gdt_vis )

endif (HAS_BGUI3D)

