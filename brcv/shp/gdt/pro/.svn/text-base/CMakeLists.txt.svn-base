# This is contrib/dbmsh3d/gdt/pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
IF (HAS_BGUI3D)

SET ( gdt_vis_sources
   gdt_process.cxx              gdt_process.h
   gdt_cmdpara.cxx              gdt_cmdpara.h

)

ADD_LIBRARY( gdt_pro ${gdt_vis_sources})

TARGET_LINK_LIBRARIES( gdt_pro dbmsh3d_pro dbmsh3d gdt )

ENDIF (HAS_BGUI3D)

