# breye1/CMakeLists.txt
# Brown Eyes package.

include_directories( ${VXL_VGUI_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories(${VXD_BRLD_INCLUDE_DIR})

include_directories( ${LEMS_SOURCE_DIR}/db3p )
include_directories( ${LEMS_SOURCE_DIR}/algo )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/basic )

# Development Visualizer
add_subdirectory(dbvis1)

# Virtual Object Recognition Lab
add_subdirectory(vorl1)
