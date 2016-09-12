# playland/sadal/CMakeLists.txt
# Brown personal testing area.
include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye )


include_directories( ${VXL_BRL_INCLUDE_DIR} )

include_directories(${VXL_OXL_INCLUDE_DIR} )

include_directories(${VXL_OXL_INCLUDE_DIR}/mvl )

include_directories(${VXL_RPL_INCLUDE_DIR} )

include( ${LEMS_CMAKE_DIR}/Modules/UseBGUI3D.cmake )
# my bvis based GUI
subdirs( gui )


subdirs( bprt )

subdirs( bfac )


