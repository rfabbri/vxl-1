# This is brcv/shp/dbsk3d/cmdproc/CMakeLists.txt
# Brown Shock3D Command-Line Processing Library

include_directories( ${LEMS_SOURCE_DIR}/brcv/shp)

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

if (HAS_BGUI3D)

set(dbsk3d_cmdproc_sources
   dbsk3d_provis_bktseg.cxx

   dbsk3d_process_vis.cxx           dbsk3d_process_vis.h

   dbsk3d_cmdproc.cxx               dbsk3d_cmdproc.h
)


add_library(dbsk3d_cmdproc ${dbsk3d_cmdproc_sources})

target_link_libraries(dbsk3d_cmdproc dbsk3d dbsk3d_algo dbsk3d_pro dbsk3d_vis dbmsh3d_vis)

endif (HAS_BGUI3D)
