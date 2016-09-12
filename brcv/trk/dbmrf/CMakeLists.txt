# brcv/trk/dbmrf/CMakeLists.txt
# Brown Markov Random Field Tracking

include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_GEL_INCLUDE_DIR} )

# Processing Library
subdirs(pro)

# Visualization Library
subdirs(vis)

if(BUILD_TESTING)
#  subdirs(tests)
endif(BUILD_TESTING)
