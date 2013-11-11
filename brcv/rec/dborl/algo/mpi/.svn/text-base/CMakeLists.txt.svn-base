# If we're building for a cluster, add mpi include directory and the proc classes.
# The "FindMPI" cmake include does the work including Windows vs. Linux

OPTION (BUILD_dborl_cluster "Build dborl cluster utilities using MPI" NO)
IF (BUILD_dborl_cluster)

INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic)
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/shp)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

OPTION (MPI_CPP_BINDING "Use the C++ bindings for MPI" NO)
INCLUDE (${LEMS_CMAKE_DIR}/Modules/FindMPI.cmake)
INCLUDE_DIRECTORIES (${MPI_INCLUDE_DIR})
IF (MPI_CCFLAGS)
   ADD_DEFINITIONS (${MPI_CCFLAGS})
ENDIF (MPI_CCFLAGS)
LINK_DIRECTORIES (${MPI_LIBRARY_DIR})
SET (dborl_cluster_src
  dborl_cluster.h dborl_cluster.cxx
  dborl_processor_base.h
  dborl_dataset_processor.h 
)  

#AUX_SOURCE_DIRECTORY(Templates dborl_cluster_src)

ADD_LIBRARY(dborl_mpi ${dborl_cluster_src})
TARGET_LINK_LIBRARIES(dborl_mpi dborl_algo bxml vcl)
IF(BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF(BUILD_EXAMPLES)

SUBDIRS(shock)

SUBDIRS(retrieval)

SUBDIRS( symseg )

SUBDIRS( compute_ishock )

ENDIF (BUILD_dborl_cluster)

