# breye1/CMakeLists.txt
# Brown Eyes package.

include_directories( ${VXL_VGUI_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )

# Brown Process Library
add_subdirectory( bpro1 )

# Video Process Library
add_subdirectory( vidpro1 )

# Visualizer (will move to brl someday)
add_subdirectory( bvis1 )

# Development Visualizer
add_subdirectory( dbvis1 )

# Virtual Object Recognition Lab
add_subdirectory( vorl1 )

#To be compatible with latest verison of vxl copy over library
add_subdirectory( vidl1 )


