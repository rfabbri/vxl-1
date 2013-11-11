# breye/CMakeLists.txt
# Brown Eyes package.

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas/brdb )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

# Brown Process Library
SUBDIRS( bpro )

# Video Process Library
SUBDIRS( vidpro )

# Visualizer (will move to brl someday)
SUBDIRS( bvis )

# Development Visualizer
SUBDIRS( dbvis )

# Virtual Object Recognition Lab
SUBDIRS( vorl )

