# (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
# No part of this software may be used, reproduced or transmitted for
# any purpose, in any form or by any means, except as expressly
# permitted in writing by Bio-Tree Systems, Inc.


# If we're building for a cluster, add mpi include directory and the proc classes.
# The "FindMPI" cmake include does the work including Windows vs. Linux
option (BUILD_CONTRIB_biotree_cluster "Build biotree filter processing for clusters" NO)
if (BUILD_CONTRIB_biotree_cluster)
  include_directories(${lems_SOURCE_DIR}/basic)
  include_directories(${lems_SOURCE_DIR}/db3p)
  include_directories(${lems_SOURCE_DIR}/brcv/mvg ${biotree_SOURCE_DIR}/resc)
  include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
  include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

  option (MPI_CPP_BINDING "Use the C++ bindings for MPI" NO)
  include (${LEMS_CMAKE_DIR}/Modules/FindMPI.cmake)
  include_directories (${MPI_INCLUDE_DIR})
  if (MPI_CCFLAGS)
      add_definitions (${MPI_CCFLAGS})
  endif (MPI_CCFLAGS)
  
  set (biocluster_src  
    biocluster_volpart_filtering_proc.txx biocluster_volpart_filtering_proc.h
  )
    
  add_definitions (-DBUILD_CONTRIB_biotree_cluster)

  aux_source_directory(Templates biocluster_src)

  add_library(biocluster ${biocluster_src})

  target_link_libraries(biocluster vcl vgl vsl bioproc imgr imgr_file_format xmvg xmvg_algo xmvg_io biob )

  if(BUILD_EXAMPLES)
    subdirs(examples)
  endif(BUILD_EXAMPLES)
endif (BUILD_CONTRIB_biotree_cluster)
