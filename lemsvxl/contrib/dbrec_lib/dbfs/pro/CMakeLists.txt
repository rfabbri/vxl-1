# This is contrib/dbfs/pro/CMakeLists.txt
# Brown - LEMS Feature Selection Processes Library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/brec )

include_directories( ${LEMS_SOURCE_DIR}/contrib/dbrec_lib )

set(dbfs_pro_sources
    dbfs_register.h      dbfs_register.cxx
    dbfs_processes.h
   )

aux_source_directory(Templates dbfs_pro_sources)

aux_source_directory(processes dbfs_pro_sources)

add_library(dbfs_pro ${dbfs_pro_sources})

target_link_libraries(dbfs_pro dbfs )

#if( BUILD_TESTING )
#  add_subdirectory(tests)
#endif( BUILD_TESTING )

