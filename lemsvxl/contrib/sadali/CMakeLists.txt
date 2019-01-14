# contrib/sadali/CMakeLists.txt
# Brown personal testing area.

include_directories( ${LEMS_SOURCE_DIR}/algo )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/contrib/sadali )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
#include_directories( ${VXL_OXL_INCLUDE_DIR}/mvl )
include_directories( ${VXL_RPL_INCLUDE_DIR} )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

# my bvis1 based GUI
add_subdirectory( gui )


add_subdirectory( bprt )

add_subdirectory( bfac )


add_subdirectory(dbbbopt_vis)

add_subdirectory(dbhom_vis)


