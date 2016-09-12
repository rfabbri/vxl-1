# This is contrib/anil/bin/CMakeLists.txt
# Brown Shock Recognition Library Executables

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

add_subdirectory( dborl_bin )
