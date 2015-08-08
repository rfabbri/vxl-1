# This is brcv/shp/dbskfg/pro/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )


SET(shape_align_sources

  #algos
  dbskr_align_shapes.h dbskr_align_shapes.cxx

)

ADD_LIBRARY(shape_align ${shape_align_sources})


