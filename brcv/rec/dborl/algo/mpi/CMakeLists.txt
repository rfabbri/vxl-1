# If we're building for a cluster, add mpi include directory and the proc classes.
# The "FindMPI" cmake include does the work including Windows vs. Linux

option (BUILD_dborl_cluster "Build dborl cluster utilities using MPI" NO)
if (BUILD_dborl_cluster)

include_directories(${lems_SOURCE_DIR}/basic)
include_directories(${lems_SOURCE_DIR}/brcv/rec)
include_directories(${lems_SOURCE_DIR}/brcv/trk )
include_directories(${lems_SOURCE_DIR}/brcv/shp)
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

option (MPI_CPP_BINDING "Use the C++ bindings for MPI" NO)
include (${LEMS_CMAKE_DIR}/Modules/FindMPI.cmake)
include_directories (${MPI_INCLUDE_DIR})
if (MPI_CCFLAGS)
   add_definitions (${MPI_CCFLAGS})
endif (MPI_CCFLAGS)
link_directories (${MPI_LIBRARY_DIR})
set (dborl_cluster_src
  dborl_cluster.h dborl_cluster.cxx
  dborl_processor_base.h
  dborl_dataset_processor.h 
)  

#aux_source_directory(Templates dborl_cluster_src)

add_library(dborl_mpi ${dborl_cluster_src})
target_link_libraries(dborl_mpi dborl_algo bxml vcl)
if(BUILD_EXAMPLES)
  subdirs(examples)
endif(BUILD_EXAMPLES)

subdirs(shock)

subdirs(retrieval)

subdirs( symseg )

subdirs( compute_ishock )

endif (BUILD_dborl_cluster)

