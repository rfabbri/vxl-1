# breye/CMakeLists.txt
# Brown Eyes package.

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas/brdb )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/basic )

# Brown Process Library
subdirs( bpro )

# Video Process Library
subdirs( vidpro )

# Visualizer (will move to brl someday)
subdirs( bvis )

# Development Visualizer
subdirs( dbvis )

# Virtual Object Recognition Lab
subdirs( vorl )

