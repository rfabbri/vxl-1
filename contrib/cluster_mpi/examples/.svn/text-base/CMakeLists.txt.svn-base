# /contrib/ozge/cluster/examples/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/src_pp )

ADD_EXECUTABLE (cluster_shock_matching cluster_shock_matching.cxx)
#ADD_EXECUTABLE (cluster_simple cluster_simple.cxx)

INCLUDE (${LEMS_CMAKE_DIR}/Modules/FindMPI.cmake)
INCLUDE_DIRECTORIES (${MPI_INCLUDE_DIR})
IF (MPI_CCFLAGS)
  ADD_DEFINITIONS (${MPI_CCFLAGS})
ENDIF (MPI_CCFLAGS)

LINK_DIRECTORIES (${MPI_LIBRARY_DIR})

TARGET_LINK_LIBRARIES (cluster_shock_matching cluster_mpi dbskr dbsk2d_pro dbru_algo vil btol brip vidpro ${MPI_LIBRARIES})
#TARGET_LINK_LIBRARIES (cluster_simple ${MPI_LIBRARIES})

IF (MPI_LINKFLAGS)
  SET_TARGET_PROPERTIES (cluster_shock_matching PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
ENDIF (MPI_LINKFLAGS)


    

