# basic/dbil/vis/CMakeLists.txt
# Development Brown Image Visualization Extention Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

IF (VXL_VGUI_FOUND)

SET(dbil_vis_sources 

#tools

   dbil_image_tool.h                       dbil_image_tool.cxx
   dbil_image_intensity_inspector.h        dbil_image_intensity_inspector.cxx
   dbil_image_patch_gradient_tool.h        dbil_image_patch_gradient_tool.cxx
   dbil_image_region_stats_tool.h          dbil_image_region_stats_tool.cxx

   dbil_image_observer.h
)


#AUX_SOURCE_DIRECTORY(Templates bvis1_sources)

ADD_LIBRARY(dbil_vis ${dbil_vis_sources})

TARGET_LINK_LIBRARIES(dbil_vis bvis1 bil_algo vgl)

ENDIF (VXL_VGUI_FOUND)
