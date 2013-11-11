# (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
# No part of this software may be used, reproduced or transmitted for
# any purpose, in any form or by any means, except as expressly
# permitted in writing by Bio-Tree Systems, Inc.
    
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/biotree/xmvg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

IF (BUILD_CONTRIB_biotree_cluster)
    ADD_EXECUTABLE (biocluster_volpart_example_xml biocluster_volpart_filtering_example_xml.cxx)
    ADD_EXECUTABLE (biocluster_splr_filtering biocluster_splr_filtering.cxx)
    INCLUDE (${LEMS_CMAKE_DIR}/Modules/FindMPI.cmake)
    INCLUDE_DIRECTORIES (${MPI_INCLUDE_DIR})
    IF (MPI_CCFLAGS)
        ADD_DEFINITIONS (${MPI_CCFLAGS})
    ENDIF (MPI_CCFLAGS)
    LINK_DIRECTORIES (${MPI_LIBRARY_DIR})
    TARGET_LINK_LIBRARIES (biocluster_volpart_example_xml biocluster bioproc bioproc_io xmvg ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (biocluster_splr_filtering bioproc biob bioproc_io xmvg ${MPI_LIBRARIES})
    IF (MPI_LINKFLAGS)
        SET_TARGET_PROPERTIES (biocluster_volpart_example_xml PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (biocluster_splr_filtering PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    ENDIF (MPI_LINKFLAGS)
    
ENDIF(BUILD_CONTRIB_biotree_cluster)


