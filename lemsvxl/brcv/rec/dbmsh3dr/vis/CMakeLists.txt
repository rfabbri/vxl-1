# rec/dbmsh3dr/vis/CMakeLists.txt
# visualization package for dbmsh3dr.

include_directories( ${lems_SOURCE_DIR}/brcv/shp)
include_directories( ${lems_SOURCE_DIR}/brcv/rec)

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

if (HAS_BGUI3D)


set(dbmsh3dr_vis_sources

dbmsh3dr_process_vis.cxx         dbmsh3dr_process_vis.h
dbmsh3dr_vis.cxx                 dbmsh3dr_vis.h
dbmsh3dr_cmdproc.cxx             dbmsh3dr_cmdproc.h

)

add_library(dbmsh3dr_vis  ${dbmsh3dr_vis_sources})

target_link_libraries( dbmsh3dr_vis dbmsh3dr_pro dbmsh3d_vis dbmsh3d_cmdproc )  

endif (HAS_BGUI3D)

