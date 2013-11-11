# dbrl/CMakeLists.txt
# Mutual dependencies:  bbas --> bseg --> vvid  and  bbas --> bmvl

PROJECT( dbrl )

doxygen_add_package(contrib/dbrl "Brown University Development Libraries")

# In contrib/dbrl there is sometimes an "extra" level in the directory structure
# (.../contrib/dbrl/{bbas,bseg,bmvl}/foo/foo.h)
# Setting DBRL_INCLUDE_DIR in this way allows #include <foo/foo.h> without
# having to know the internal directory structure.
# The root directory should be listed last in case anyone tries to append to this path
SET (kdu_INCLUDE_DIR
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
SET( DBRL_INCLUDE_DIR "${dbrl_SOURCE_DIR}/dbbas;${dbrl_SOURCE_DIR}/bcl;${dbrl_BINARY_DIR}/dbbas;${dbrl_SOURCE_DIR}/dbseg;${dbrl_SOURCE_DIR}/bmvl;${dbrl_SOURCE_DIR}/bpro;${kdu_INCLUDE_DIR};" CACHE STATIC "Brown University package include files" FORCE )

SET( dbrl_BINARY_DIR "${dbrl_BINARY_DIR}" )
SET( dbrl_INSTALL_INCLUDE_DIR 
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/dbrl
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bbas
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/dbrl/dbbas
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bseg
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/dbrl/dbseg
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/brl/bvml
  CACHE INTERNAL "Brown University package include files"
)
FIND_PACKAGE(VXL)
SET( DBRL_MODULE_PATH ${dbrl_SOURCE_DIR}/dbmods )
SET( BRL_MODULE_PATH ${brl_SOURCE_DIR}/bmods )
OPTION(LINK_WITH_DLIBS "Link Brown Development Projects with Development Libraries?" YES)
OPTION(BUILD_OCTI_TESTING "Build tests that interest Octavian?" NO)

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${DBRL_INCLUDE_DIR} )

# basic services package
SUBDIRS(dbbas)

IF( BUILD_GEL )
  # segmentation package
SUBDIRS(dbseg)

  # video view library
 # SUBDIRS(dvvid)
ENDIF(BUILD_GEL)

# multiview library
#SUBDIRS(dbmvl)

# standalone executables
#SUBDIRS(dbexe)

# process libraries

#3rd party libraries
#SUBDIRS(b3p)


#SUBDIRS(dbbkmdl)

#SUBDIRS(dJ2KTest)

#platform-independent includes for dbrl
#SUBDIRS(dbcl)

