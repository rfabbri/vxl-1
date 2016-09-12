# contrib/sadali/CMakeLists.txt
# Brown personal testing area.

include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/contrib/sadali )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
#include_directories( ${VXL_OXL_INCLUDE_DIR}/mvl )
include_directories( ${VXL_RPL_INCLUDE_DIR} )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

# my bvis1 based GUI
subdirs( gui )


subdirs( bprt )

subdirs( bfac )


subdirs(dbbbopt_vis)

subdirs(dbhom_vis)


