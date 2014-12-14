# breye1/CMakeLists.txt
# Brown Eyes package.

INCLUDE_DIRECTORIES( ${VXL_VGUI_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

# Brown Process Library
SUBDIRS( bpro1 )

# Video Process Library
SUBDIRS( vidpro1 )

# Visualizer (will move to brl someday)
SUBDIRS( bvis1 )

# Development Visualizer
SUBDIRS( dbvis1 )

# Virtual Object Recognition Lab
SUBDIRS( vorl1 )

#To be compatible with latest verison of vxl copy over library
SUBDIRS( vidl1 )


