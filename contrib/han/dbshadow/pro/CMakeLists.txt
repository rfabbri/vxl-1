# This is 
# Brown xxxxxxx Processing library
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR} )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/breye )

set(dbshadow_pro_sources
   dbshadow_process.cxx   dbshadow_process.h
   
)



add_library(dbshadow_pro ${dbshadow_pro_sources})

target_link_libraries(dbshadow_pro  vidpro dbecl bmrf vgl vgl_algo brip )


if(BUILD_TESTING)
#  subdirs(tests)
endif(BUILD_TESTING)

