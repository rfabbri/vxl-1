# bdirl information-theoretic resistration library
# blem/brcv/mvg/dbirl/CMakeLists.txt
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )

set(dbirl_sources
    dbirl_h_computation.cxx           dbirl_h_computation.h
)

add_library(dbirl ${dbirl_sources})


target_link_libraries(dbirl dbinfo sdet bsta brip bugl bsol btol bdgl vsol vil vgl_algo vnl vsl vbl_io vbl)

# Processing Library
add_subdirectory(pro)

# Command line app
add_subdirectory(app)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

