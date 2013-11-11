# rec/dbskr/vis/CMakeLists.txt
# visualization package for dbskr.

INCLUDE_DIRECTORIES(${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)


INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET(dbsk3dr_vis_sources

dbsk3dr_process_vis.cxx         dbsk3dr_process_vis.h
dbsk3dr_draw.cxx                dbsk3dr_draw.h
dbsk3dr_cmdproc.cxx             dbsk3dr_cmdproc.h
dbsk3dr_provis_smre.cxx

)

ADD_LIBRARY(dbsk3dr_vis  ${dbsk3dr_vis_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbsk3dr_vis dbsk3dr_pro dbsk3d_vis )  


ENDIF (HAS_BGUI3D)
