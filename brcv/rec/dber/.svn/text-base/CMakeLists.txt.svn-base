# Brown Edge-based Recognition Library  
# brcv/rec/dber/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SET(dber_sources
dber_match.h            dber_match.cxx
dber_edgel_similarity.h dber_edgel_similarity.cxx
dber_utilities.h        dber_utilities.cxx
)

AUX_SOURCE_DIRECTORY(Templates dber_sources)

ADD_LIBRARY(dber  ${dber_sources})

TARGET_LINK_LIBRARIES(dber vnl dbsol dbru)

# algorithms library
#SUBDIRS(algo)

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

#IF(BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF(BUILD_TESTING)

# Executables
#SUBDIRS(bin)
