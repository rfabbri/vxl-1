# This is contrib/dbvxm/bin/CMakeLists.txt
# Command Line Executables

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )


ADD_EXECUTABLE(dbvxm_cmd_line_utilities
dbvxm_cmd_line.cxx
dbvxm_cmd_line.h
dbvxm_cmd_line_main.cxx
)

TARGET_LINK_LIBRARIES( dbvxm_cmd_line_utilities dbvxm bvxm_rec vul vil bvgl vil_io vil_pro )
