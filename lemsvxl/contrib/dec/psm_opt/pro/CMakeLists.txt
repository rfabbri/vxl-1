# This is lemsvxlsrc/contrib/dec/psm_opt/pro/CMakeLists.txt
# psm_opts's Process Library

if(VXL_EXPAT_FOUND)
include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${lems_SOURCE_DIR}/contrib/dec )

set(psm_opt_pro_sources
    psm_opt_processes.h
    psm_opt_register.h  psm_opt_register.cxx   
)

aux_source_directory(processes psm_opt_pro_sources)

add_library(psm_opt_pro ${psm_opt_pro_sources})

target_link_libraries(psm_opt_pro psm psm_io psm_algo psm_opt bprb vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vil vil_io vil_pro vnl vnl_io bgeo vgl brdb)

endif(VXL_EXPAT_FOUND)
