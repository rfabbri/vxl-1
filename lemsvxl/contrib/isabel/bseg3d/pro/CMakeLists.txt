# This is lemsvxlsrc/contrib/dec/bseg3d/pro/CMakeLists.txt
# bseg3d's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
if(VXL_EXPAT_FOUND)
include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/bprb )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${LEMS_SOURCE_DIR}/contrib/dec )

set(bseg3d_pro_sources
  bseg3d_processes.h
  bseg3d_register.h   bseg3d_register.cxx
)

aux_source_directory(Templates bseg3d_pro_sources)
aux_source_directory(processes bseg3d_pro_sources)

add_library(bseg3d_pro ${bseg3d_pro_sources})

target_link_libraries(bseg3d_pro bseg3d)

#if( BUILD_TESTING )
#  add_subdirectory(tests)
#endif( BUILD_TESTING )
endif(VXL_EXPAT_FOUND)
