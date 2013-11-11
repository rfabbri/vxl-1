# brcv/trk/dbmrf/CMakeLists.txt
# Brown Markov Random Field Tracking

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
