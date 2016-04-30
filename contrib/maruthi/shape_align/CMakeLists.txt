# This is brcv/shp/dbskfg/pro/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR})

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${CMAKE_VLFEAT_INCLUDES} )

LINK_DIRECTORIES(${CMAKE_VLFEAT_LIBS})

SET(shape_align_sources

  #algos
  dbskr_align_shapes.h dbskr_align_shapes.cxx

)

SET(train_sources

  #algos
  dbskr_train_routines.h dbskr_train_routines.cxx 
)

ADD_LIBRARY(shape_align ${shape_align_sources})
ADD_LIBRARY(train_routines ${train_sources})

ADD_EXECUTABLE (ed_match_exec ed_match_exec.cxx )
TARGET_LINK_LIBRARIES ( ed_match_exec shape_align dbskr dbskr_algo)

ADD_EXECUTABLE (train_exec train_exec.cxx )
TARGET_LINK_LIBRARIES ( train_exec train_routines dbskr dbskr_algo vl)
