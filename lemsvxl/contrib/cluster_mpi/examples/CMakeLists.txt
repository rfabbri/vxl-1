# /contrib/ozge/cluster/examples/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/brcv/rec/ )
include_directories( ${LEMS_SOURCE_DIR}/contrib )
#include_directories( ${LEMS_SOURCE_DIR}/db3p/expatpp/expat/lib )
#include_directories( ${LEMS_SOURCE_DIR}/db3p/expatpp/src_pp )

add_executable (cluster_shock_matching cluster_shock_matching.cxx)
#add_executable (cluster_simple cluster_simple.cxx)

include (${LEMS_CMAKE_DIR}/Modules/FindMPI.cmake)
include_directories (${MPI_INCLUDE_DIR})
if (MPI_CCFLAGS)
  add_definitions (${MPI_CCFLAGS})
endif (MPI_CCFLAGS)

link_directories (${MPI_LIBRARY_DIR})

target_link_libraries (cluster_shock_matching cluster_mpi dbskr dbsk2d_pro dbru_algo vil btol brip vidpro ${MPI_LIBRARIES})
#target_link_libraries (cluster_simple ${MPI_LIBRARIES})

if (MPI_LINKFLAGS)
  set_target_properties (cluster_shock_matching PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
endif (MPI_LINKFLAGS)


    

