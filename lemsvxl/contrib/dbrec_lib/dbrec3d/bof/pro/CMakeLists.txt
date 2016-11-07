# This is contribdbrec_lib/dbrec3d/pro/CMakeLists.txt
# Brown - LEMS bag of features Process Library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/boxm )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/bvpl )


include_directories( ${lems_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d/bof )

set(bof_pro_sources
    bof_register.h      bof_register.cxx
    bof_processes.h
    )

#aux_source_directory(Templates dbrec3d_pro_sources)

aux_source_directory(processes bof_pro_sources)

add_library(bof_pro ${bof_pro_sources})

target_link_libraries(bof_pro bof boxm boxm_algo boct bprb brdb bvpl_octree)

#if( BUILD_TESTING )
#  add_subdirectory(tests)
#endif( BUILD_TESTING )

