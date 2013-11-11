# This is brcv/shp/dbsk3d/cmdproc/CMakeLists.txt
# Brown Shock3D Command-Line Processing Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET(dbsk3d_cmdproc_sources
   dbsk3d_provis_bktseg.cxx

   dbsk3d_process_vis.cxx           dbsk3d_process_vis.h

   dbsk3d_cmdproc.cxx               dbsk3d_cmdproc.h
)


ADD_LIBRARY(dbsk3d_cmdproc ${dbsk3d_cmdproc_sources})

TARGET_LINK_LIBRARIES(dbsk3d_cmdproc dbsk3d dbsk3d_algo dbsk3d_pro dbsk3d_vis dbmsh3d_vis)

ENDIF (HAS_BGUI3D)
