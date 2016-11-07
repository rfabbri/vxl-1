# This is brcv/shp/dbsk3d/CMakeLists.txt
# Brown Shock3D Library

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/shp)

set(dbsk3d_sources

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

add_library(dbsk3d ${dbsk3d_sources})

target_link_libraries( dbsk3d dbmsh3d dbgl )  

# Algorithm Library
add_subdirectory(algo)

# FlowSheet Shock Detection Library (Frederic's Code)
add_subdirectory(flowsheet)

# Processing Library
add_subdirectory(pro)

# Visualization Library
add_subdirectory(vis)

# Command-line Processing Library
add_subdirectory(cmdproc)

if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)
