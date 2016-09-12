# This is brcv/shp/dbskfg/pro/CMakeLists.txt

include_directories(${VXL_GEL_INCLUDE_DIR})
include_directories(${VXL_BRL_INCLUDE_DIR})

include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib/maruthi )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${CMAKE_VLFEAT_INCLUDES} )

link_directories(${CMAKE_VLFEAT_LIBS})

set(shape_align_sources

  #algos
  dbskr_align_shapes.h dbskr_align_shapes.cxx

)

set(train_sources

  #algos
  dbskr_train_routines.h dbskr_train_routines.cxx   
)

SET(test_sources

  #algos
  dbskr_test_routines.h dbskr_test_routines.cxx   
)

add_library(shape_align ${shape_align_sources})
add_library(train_routines ${train_sources})
add_library(test_routines ${test_sources})

add_executable (ed_match_exec ed_match_exec.cxx )
target_link_libraries ( ed_match_exec shape_align dbskr dbskr_algo)

ADD_EXECUTABLE (train_exec train_exec.cxx )
TARGET_LINK_LIBRARIES ( train_exec train_routines dbskr dbskr_algo vl)

add_executable (test_exec test_exec.cxx )
target_link_libraries ( test_exec test_routines dbskr dbskr_algo vl)
