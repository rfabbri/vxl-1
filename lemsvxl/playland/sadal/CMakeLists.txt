# playland/sadal/CMakeLists.txt
# Brown personal testing area.
include_directories( ${LEMS_SOURCE_DIR}/algo )
include_directories( ${LEMS_SOURCE_DIR}/breye )


include_directories( ${VXL_BRL_INCLUDE_DIR} )

include_directories(${VXL_OXL_INCLUDE_DIR} )

include_directories(${VXL_OXL_INCLUDE_DIR}/mvl )

include_directories(${VXL_RPL_INCLUDE_DIR} )

include( ${LEMS_CMAKE_DIR}/Modules/UseBGUI3D.cmake )
# my bvis based GUI
add_subdirectory( gui )


add_subdirectory( bprt )

add_subdirectory( bfac )


