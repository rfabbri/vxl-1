# (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
# No part of this software may be used, reproduced or transmitted for
# any purpose, in any form or by any means, except as expressly
# permitted in writing by Bio-Tree Systems, Inc.
    
include_directories( ${lems_SOURCE_DIR}/contrib/biotree/xmvg)
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

if (BUILD_CONTRIB_biotree_cluster)
    add_executable (biocluster_volpart_example_xml biocluster_volpart_filtering_example_xml.cxx)
    add_executable (biocluster_splr_filtering biocluster_splr_filtering.cxx)
    include (${LEMS_CMAKE_DIR}/Modules/FindMPI.cmake)
    include_directories (${MPI_INCLUDE_DIR})
    if (MPI_CCFLAGS)
        add_definitions (${MPI_CCFLAGS})
    endif (MPI_CCFLAGS)
    link_directories (${MPI_LIBRARY_DIR})
    target_link_libraries (biocluster_volpart_example_xml biocluster bioproc bioproc_io xmvg ${MPI_LIBRARIES})
    target_link_libraries (biocluster_splr_filtering bioproc biob bioproc_io xmvg ${MPI_LIBRARIES})
    if (MPI_LINKFLAGS)
        set_target_properties (biocluster_volpart_example_xml PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (biocluster_splr_filtering PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    endif (MPI_LINKFLAGS)
    
endif(BUILD_CONTRIB_biotree_cluster)


