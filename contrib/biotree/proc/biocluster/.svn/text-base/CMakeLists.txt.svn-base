# (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
# No part of this software may be used, reproduced or transmitted for
# any purpose, in any form or by any means, except as expressly
# permitted in writing by Bio-Tree Systems, Inc.


# If we're building for a cluster, add mpi include directory and the proc classes.
# The "FindMPI" cmake include does the work including Windows vs. Linux
OPTION (BUILD_CONTRIB_biotree_cluster "Build biotree filter processing for clusters" NO)
IF (BUILD_CONTRIB_biotree_cluster)
  INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic)
  INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/db3p)
  INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/mvg ${biotree_SOURCE_DIR}/resc)
  INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
  INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

  OPTION (MPI_CPP_BINDING "Use the C++ bindings for MPI" NO)
  INCLUDE (${LEMS_CMAKE_DIR}/Modules/FindMPI.cmake)
  INCLUDE_DIRECTORIES (${MPI_INCLUDE_DIR})
  IF (MPI_CCFLAGS)
      ADD_DEFINITIONS (${MPI_CCFLAGS})
  ENDIF (MPI_CCFLAGS)
  
  SET (biocluster_src  
    biocluster_volpart_filtering_proc.txx biocluster_volpart_filtering_proc.h
  )
    
  ADD_DEFINITIONS (-DBUILD_CONTRIB_biotree_cluster)

  AUX_SOURCE_DIRECTORY(Templates biocluster_src)

  ADD_LIBRARY(biocluster ${biocluster_src})

  TARGET_LINK_LIBRARIES(biocluster vcl vgl vsl bioproc imgr imgr_file_format xmvg xmvg_algo xmvg_io biob )

  IF(BUILD_EXAMPLES)
    SUBDIRS(examples)
  ENDIF(BUILD_EXAMPLES)
ENDIF (BUILD_CONTRIB_biotree_cluster)
