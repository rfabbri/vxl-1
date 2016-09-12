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

set(dbcri_pro_sources
   dbcri_process.cxx   dbcri_process.h
   
)

#aux_source_directory(Templates dbecl_pro_sources)

add_library(dbcri_pro ${dbcri_pro_sources})

target_link_libraries(dbcri_pro  vidpro dbecl bmrf vgl vgl_algo)


if(BUILD_TESTING)
#  subdirs(tests)
endif(BUILD_TESTING)

