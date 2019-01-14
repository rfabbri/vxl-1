include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_GEL_INCLUDE_DIR})
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )

add_executable( ps_xz ps_xz.cxx ps_xz.h )
target_link_libraries( ps_xz dbdet bdgl )
