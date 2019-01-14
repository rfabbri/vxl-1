# This is brcv/shp/dbmsh3d/algovtk/pro/CMakeLists.txt
# Processes for Brown Mesh Library

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )

set(dbmsh3d_algovtk_pro_sources
   dbmsh3d_isosurface_of_seg3d_process.h       dbmsh3d_isosurface_of_seg3d_process.cxx
   dbmsh3d_isosurface_process.h                dbmsh3d_isosurface_process.cxx
)

add_library(dbmsh3d_algovtk_pro  ${dbmsh3d_algovtk_pro_sources})

target_link_libraries(dbmsh3d_algovtk_pro dbmsh3d_algovtk dbmsh3d_algo vidpro1 vul)


