# rec/dbskr/vis/CMakeLists.txt
# visualization package for dbskr.

include_directories(${VXL_RPL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/shp)


include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

if (HAS_BGUI3D)

set(dbsk3dr_vis_sources

dbsk3dr_process_vis.cxx         dbsk3dr_process_vis.h
dbsk3dr_draw.cxx                dbsk3dr_draw.h
dbsk3dr_cmdproc.cxx             dbsk3dr_cmdproc.h
dbsk3dr_provis_smre.cxx

)

add_library(dbsk3dr_vis  ${dbsk3dr_vis_sources})

# add the required libraries into this list
target_link_libraries( dbsk3dr_vis dbsk3dr_pro dbsk3d_vis )  


endif (HAS_BGUI3D)
