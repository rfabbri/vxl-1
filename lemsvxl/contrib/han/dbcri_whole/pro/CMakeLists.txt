# This is 
# Brown xxxxxxx Processing library
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bmvl )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )

set(dbcri_whole_pro_sources
   dbcri_whole_process.cxx   dbcri_whole_process.h
  dbcri_whole_3D.cxx
   
)



add_library(dbcri_whole_pro ${dbcri_whole_pro_sources})

target_link_libraries(dbcri_whole_pro  vidpro1 dbecl bmrf vgl vgl_algo brip  brct )


if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)

