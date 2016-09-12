# dbrl/CMakeLists.txt
# Mutual dependencies:  bbas --> bseg --> vvid  and  bbas --> bmvl

project( dbrl )

doxygen_add_package(contrib/dbrl "Brown University Development Libraries")

# In contrib/dbrl there is sometimes an "extra" level in the directory structure
# (.../contrib/dbrl/{bbas,bseg,bmvl}/foo/foo.h)
# Setting DBRL_INCLUDE_DIR in this way allows #include <foo/foo.h> without
# having to know the internal directory structure.
# The root directory should be listed last in case anyone tries to append to this path
set (kdu_INCLUDE_DIR
	${dbrl_SOURCE_DIR}/dbbas/dkdu/coresys/common
    ${dbrl_SOURCE_DIR}/dbbas/dkdu/coresys/transform
	${dbrl_SOURCE_DIR}/dbbas/dkdu/coresys/roi
	${dbrl_SOURCE_DIR}/dbbas/dkdu/coresys/parameters
	${dbrl_SOURCE_DIR}/dbbas/dkdu/coresys/compressed
	${dbrl_SOURCE_DIR}/dbbas/dkdu/coresys/coding
	${dbrl_SOURCE_DIR}/dbbas/dkdu/apps/kdu_compress
	${dbrl_SOURCE_DIR}/dbbas/dkdu/apps/kdu_expand
	${dbrl_SOURCE_DIR}/dbbas/dkdu/apps/args
	${dbrl_SOURCE_DIR}/dbbas/dkdu/apps/image
	${dbrl_SOURCE_DIR}/dbbas/dkdu/apps/jp2
	${dbrl_SOURCE_DIR}/dbbas/dkdu/apps/compressed_io
)
set( DBRL_INCLUDE_DIR "${dbrl_SOURCE_DIR}/dbbas;${dbrl_SOURCE_DIR}/bcl;${dbrl_BINARY_DIR}/dbbas;${dbrl_SOURCE_DIR}/dbseg;${dbrl_SOURCE_DIR}/bmvl;${dbrl_SOURCE_DIR}/bpro;${kdu_INCLUDE_DIR};" CACHE STATIC "Brown University package include files" FORCE )

set( dbrl_BINARY_DIR "${dbrl_BINARY_DIR}" )
set( dbrl_INSTALL_INCLUDE_DIR 
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/dbrl
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bbas
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/dbrl/dbbas
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bseg
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/dbrl/dbseg
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bvml
  CACHE INTERNAL "Brown University package include files"
)
find_package(VXL)
set( DBRL_MODULE_PATH ${dbrl_SOURCE_DIR}/dbmods )
set( BRL_MODULE_PATH ${brl_SOURCE_DIR}/bmods )
option(LINK_WITH_DLIBS "Link Brown Development Projects with Development Libraries?" YES)
option(BUILD_OCTI_TESTING "Build tests that interest Octavian?" NO)

include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${DBRL_INCLUDE_DIR} )

# basic services package
add_subdirectory(dbbas)

if( BUILD_GEL )
  # segmentation package
add_subdirectory(dbseg)

  # video view library
 # add_subdirectory(dvvid)
endif(BUILD_GEL)

# multiview library
#add_subdirectory(dbmvl)

# standalone executables
#add_subdirectory(dbexe)

# process libraries

#3rd party libraries
#add_subdirectory(b3p)


#add_subdirectory(dbbkmdl)

#add_subdirectory(dJ2KTest)

#platform-independent includes for dbrl
#add_subdirectory(dbcl)

