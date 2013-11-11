# This is brcv/shp/dbmsh3d/algovtk/pro/CMakeLists.txt
# Processes for Brown Mesh Library

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

SET(dbmsh3d_algovtk_pro_sources
   dbmsh3d_isosurface_of_seg3d_process.h       dbmsh3d_isosurface_of_seg3d_process.cxx
   dbmsh3d_isosurface_process.h                dbmsh3d_isosurface_process.cxx
)

ADD_LIBRARY(dbmsh3d_algovtk_pro  ${dbmsh3d_algovtk_pro_sources})

TARGET_LINK_LIBRARIES(dbmsh3d_algovtk_pro dbmsh3d_algovtk dbmsh3d_algo vidpro1 vul)


