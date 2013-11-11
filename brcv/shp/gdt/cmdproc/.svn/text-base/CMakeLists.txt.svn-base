# This is contrib/dbmsh3d/gdt/pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
IF (HAS_BGUI3D)

SET ( gdt_vis_sources
   gdt_cmdproc.cxx              gdt_cmdproc.h

)

ADD_LIBRARY( gdt_cmdproc ${gdt_vis_sources})

TARGET_LINK_LIBRARIES( gdt_cmdproc dbmsh3d dbmsh3d_pro gdt gdt_pro gdt_vis )

ENDIF (HAS_BGUI3D)

