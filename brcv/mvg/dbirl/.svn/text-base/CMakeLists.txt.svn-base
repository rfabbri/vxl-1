# bdirl information-theoretic resistration library
# blem/brcv/mvg/dbirl/CMakeLists.txt
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )

SET(dbirl_sources
    dbirl_h_computation.cxx           dbirl_h_computation.h
)

ADD_LIBRARY(dbirl ${dbirl_sources})


TARGET_LINK_LIBRARIES(dbirl dbinfo sdet bsta brip bugl bsol btol bdgl vsol vil vgl_algo vnl vsl vbl_io vbl)

# Processing Library
SUBDIRS(pro)

# Command line app
SUBDIRS(app)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

