# If we're building for a cluster, add mpi include directory and the proc classes.
# The "FindMPI" cmake include does the work including Windows vs. Linux
include_directories(${lems_SOURCE_DIR}/basic)
include_directories(${lems_SOURCE_DIR}/brcv/rec)
include_directories(${lems_SOURCE_DIR}/brcv/trk )
include_directories(${lems_SOURCE_DIR}/brcv/shp)
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

#option (MPI_CPP_BINDING "Use the C++ bindings for MPI" NO)
include (${LEMS_CMAKE_DIR}/Modules/FindMPI.cmake)
include_directories (${MPI_INCLUDE_DIR})
if (MPI_CCFLAGS)
   add_definitions (${MPI_CCFLAGS})
endif (MPI_CCFLAGS)
set (cluster_src
  cluster_db_matching.cxx cluster_db_matching.h
)  
add_library(cluster_mpi ${cluster_src})
target_link_libraries(cluster_mpi vcl vgl vsl )
if(BUILD_EXAMPLES)
  subdirs(examples)
endif(BUILD_EXAMPLES)

