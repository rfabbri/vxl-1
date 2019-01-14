# This is lemsvxlsrc/contrib/dec/psm/pro/CMakeLists.txt
# psm's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
if(VXL_EXPAT_FOUND)
include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/bprb )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${LEMS_SOURCE_DIR}/contrib/dec )

set(psm_pro_sources
   psm_processes.h
   psm_register.h   psm_register.cxx
)

aux_source_directory(Templates psm_pro_sources)
aux_source_directory(processes psm_pro_sources)

add_library(psm_pro ${psm_pro_sources})

target_link_libraries(psm_pro psm psm_io bprb vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vil vil_io vil_pro vnl vnl_io bgeo vgl brdb)

#if( BUILD_TESTING )
#  add_subdirectory(tests)
#endif( BUILD_TESTING )
endif(VXL_EXPAT_FOUND)
