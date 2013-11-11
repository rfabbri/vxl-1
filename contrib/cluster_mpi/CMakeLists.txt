# If we're building for a cluster, add mpi include directory and the proc classes.
# The "FindMPI" cmake include does the work including Windows vs. Linux
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic)
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/shp)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

#OPTION (MPI_CPP_BINDING "Use the C++ bindings for MPI" NO)
INCLUDE (${LEMS_CMAKE_DIR}/Modules/FindMPI.cmake)
INCLUDE_DIRECTORIES (${MPI_INCLUDE_DIR})
IF (MPI_CCFLAGS)
   ADD_DEFINITIONS (${MPI_CCFLAGS})
ENDIF (MPI_CCFLAGS)
SET (cluster_src
  cluster_db_matching.cxx cluster_db_matching.h
)  
ADD_LIBRARY(cluster_mpi ${cluster_src})
TARGET_LINK_LIBRARIES(cluster_mpi vcl vgl vsl )
IF(BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF(BUILD_EXAMPLES)

