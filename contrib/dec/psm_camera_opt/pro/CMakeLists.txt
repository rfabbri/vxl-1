# This is lemsvxlsrc/contrib/dec/psm_camera_opt_camera_opt/pro/CMakeLists.txt
# psm_camera_opt_camera_opt's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
if(VXL_EXPAT_FOUND)
include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/bprb )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${lems_SOURCE_DIR}/contrib/dec )

set(psm_camera_opt_pro_sources
   psm_camera_opt_processes.h
   psm_camera_opt_register.h   psm_camera_opt_register.cxx
)

#aux_source_directory(Templates psm_camera_opt_pro_sources)
aux_source_directory(processes psm_camera_opt_pro_sources)

add_library(psm_camera_opt_pro ${psm_camera_opt_pro_sources})

target_link_libraries(psm_camera_opt_pro psm_camera_opt bprb vpgl vil vnl vnl_io bgeo vgl brdb)

#if( BUILD_TESTING )
#  subdirs(tests)
#endif( BUILD_TESTING )
endif(VXL_EXPAT_FOUND)
