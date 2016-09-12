# breye1/CMakeLists.txt
# Brown Eyes package.

include_directories( ${VXL_VGUI_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )

# Brown Process Library
subdirs( bpro1 )

# Video Process Library
subdirs( vidpro1 )

# Visualizer (will move to brl someday)
subdirs( bvis1 )

# Development Visualizer
subdirs( dbvis1 )

# Virtual Object Recognition Lab
subdirs( vorl1 )

#To be compatible with latest verison of vxl copy over library
subdirs( vidl1 )


