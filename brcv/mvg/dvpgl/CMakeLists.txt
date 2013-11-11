# brcv/mvg/dvpgl/CMakeLists.txt
# Photogrametry Library.

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc)

#SET( dvpgl_sources
#  EMPTY
#)

#AUX_SOURCE_DIRECTORY(Templates dvpgl_sources)

#ADD_LIBRARY(dvpgl ${dvpgl_sources})
#TARGET_LINK_LIBRARIES( dvpgl vnl vnl_algo vnl_io vgl vgl_algo vgl_io vgl_xio vsl)


#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )

SUBDIRS(algo)

SUBDIRS(pro)

SUBDIRS(vis)
