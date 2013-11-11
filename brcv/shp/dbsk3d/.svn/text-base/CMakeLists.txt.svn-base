# This is brcv/shp/dbsk3d/CMakeLists.txt
# Brown Shock3D Library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/shp)

SET(dbsk3d_sources

dbsk3d_fs_vertex.cxx                  dbsk3d_fs_vertex.h
dbsk3d_fs_edge.cxx                    dbsk3d_fs_edge.h
dbsk3d_fs_face.cxx                    dbsk3d_fs_face.h

dbsk3d_fs_mesh.cxx                    dbsk3d_fs_mesh.h

dbsk3d_fs_sheet.cxx                   dbsk3d_fs_sheet.h
dbsk3d_fs_sheet_set.cxx               dbsk3d_fs_sheet_set.h

dbsk3d_fs_comp.cxx                    dbsk3d_fs_comp.h
dbsk3d_fs_comp_set.cxx                dbsk3d_fs_comp_set.h

dbsk3d_ms_node.cxx                    dbsk3d_ms_node.h
dbsk3d_ms_curve.cxx                   dbsk3d_ms_curve.h
dbsk3d_ms_sheet.cxx                   dbsk3d_ms_sheet.h

dbsk3d_ms_hypg.cxx                    dbsk3d_ms_hypg.h

dbsk3d_sg_sa.cxx                      dbsk3d_sg_sa.h

)

ADD_LIBRARY(dbsk3d ${dbsk3d_sources})

TARGET_LINK_LIBRARIES( dbsk3d dbmsh3d dbgl )  

# Algorithm Library
SUBDIRS(algo)

# FlowSheet Shock Detection Library (Frederic's Code)
SUBDIRS(flowsheet)

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

# Command-line Processing Library
SUBDIRS(cmdproc)

IF(BUILD_TESTING)
SUBDIRS(tests)
ENDIF(BUILD_TESTING)
