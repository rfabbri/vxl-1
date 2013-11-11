# rec/dbmsh3dr/vis/CMakeLists.txt
# visualization package for dbmsh3dr.

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

IF (HAS_BGUI3D)


SET(dbmsh3dr_vis_sources

dbmsh3dr_process_vis.cxx         dbmsh3dr_process_vis.h
dbmsh3dr_vis.cxx                 dbmsh3dr_vis.h
dbmsh3dr_cmdproc.cxx             dbmsh3dr_cmdproc.h

)

ADD_LIBRARY(dbmsh3dr_vis  ${dbmsh3dr_vis_sources})

TARGET_LINK_LIBRARIES( dbmsh3dr_vis dbmsh3dr_pro dbmsh3d_vis )  

ENDIF (HAS_BGUI3D)

