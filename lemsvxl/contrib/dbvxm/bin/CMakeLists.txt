# This is contrib/dbvxm/bin/CMakeLists.txt
# Command Line Executables

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
#include_directories( ${LEMS_SOURCE_DIR}/contrib/Gregistration )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/db3p )
include_directories( ${LEMS_SOURCE_DIR}/db3p/expatpp/expat/lib )


add_executable(dbvxm_cmd_line_utilities
dbvxm_cmd_line.cxx
dbvxm_cmd_line.h
dbvxm_cmd_line_main.cxx
)

target_link_libraries( dbvxm_cmd_line_utilities dbvxm bvxm_rec vul vil bvgl vil_io vil_pro )
