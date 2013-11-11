# (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
# No part of this software may be used, reproduced or transmitted for
# any purpose, in any form or by any means, except as expressly
# permitted in writing by Bio-Tree Systems, Inc.

# Add conditional for MPI C++ bindings
SET (MPI_CCFLAGS)
IF (MPI_CPP_BINDING)
    SET (MPI_CCFLAGS -DMPI_CPP_BINDING)
ENDIF (MPI_CPP_BINDING)

# Find MPI related paths and set up includes, libraries, and other defintions based
# on platform and selections.
IF (WIN32)
    # On windows platform default paths can be found in registry (assuming MPICH2 was installed)
    FIND_PATH (MPI_INCLUDE_DIR mpi.h 
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MPICH2;Path]/include" 
# added the following just to compile ok on the local machine
      "C:/Program Files (x86)/MPICH/SDK/Include/"              
      DOC "What is the path to the MPI include files?")
    FIND_PATH (MPI_LIBRARY_DIR mpi.lib 
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MPICH2;Path]/lib" 
      DOC "What is the path to the MPI libraries?")
    SET (MPI_LIBRARIES mpi.lib)
    # If C++ bindings, add library
    IF (MPI_CPP_BINDING)
        # C++ bindings need this library
        SET (MPI_LIBRARIES ${MPI_LIBRARIES} cxx.lib)
    ENDIF (MPI_CPP_BINDING)
    # The following link flag is required but generates many unresolved symbols "___delayLoadHelper2@8"
    # It must be manually specified in the "Delay loaded DLLs" Linker input dialog
    # Without this option, executables get a DLL initialization error during launch.
    # SET (MPI_LINKFLAGS /DELAYLOAD:"MPICH2MPI.DLL")
    SET (MPI_LINKFLAGS)
ELSE (WIN32)
    # Linux platform "cross-builds" for CCV cluster by default.
    OPTION (BUILD_CLUSTERING_FOR_CCV "Build cluster support for CCV" YES)
    IF (BUILD_CLUSTERING_FOR_CCV)
        # By default we use the Myrinet includes/libraries.
        FIND_PATH (MPI_INCLUDE_DIR mpi.h /opt/mpich/myrinet/gnu/include DOC "What is the path to the MPI include files?")
        FIND_PATH (MPI_LIBRARY_DIR libmpich.a /opt/mpich/myrinet/gnu/lib DOC "What is the path to the MPI libraries?")
        SET (MPI_LIBRARIES mpich)
        OPTION (BUILD_CLUSTERING_FOR_MYRINET "Build cluster support to use CCV Myrinet" YES)
        IF (BUILD_CLUSTERING_FOR_MYRINET)
            # Myrinet requires the gm libraries
            FIND_PATH (GM_LIBRARY_DIR libgm.a /opt/gm/lib DOC "What is the path to the GM libraries?")
            SET (MPI_LIBRARIES ${MPI_LIBRARIES} gm)
            SET (MPI_LIBRARY_DIR ${MPI_LIBRARY_DIR} ${GM_LIBRARY_DIR})
        ENDIF (BUILD_CLUSTERING_FOR_MYRINET)
        # These flags are used by the mpiCC/mpicc commands
    ELSE (BUILD_CLUSTERING_FOR_CCV)
        # When not cross-building, we assume that version 1.2.7 is being used.
        FIND_PATH (MPI_INCLUDE_DIR mpi.h 
          "/opt/mpich-1.2.7/include" 
          "/opt/mpich/gnu/include"
          DOC "What is the path to the MPI include files?")
        FIND_PATH (MPI_LIBRARY_DIR libmpich.a 
          "/opt/mpich-1.2.7/lib"
          "/opt/mpich/gnu/lib"
          DOC "What is the path to the MPI libraries?")
        SET (MPI_LIBRARIES mpich pthread rt)
    ENDIF (BUILD_CLUSTERING_FOR_CCV)
    SET (MPI_LINKFLAGS)
    SET (MPI_CCFLAGS ${MPI_CCFLAGS} -DUSE_STDARG -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDARG_H=1 -DUSE_STDARG=1 -DMALLOC_RET_VOID=1)
    IF (MPI_CPP_BINDING)
        # C++ bindings require additional includes, libraries, and switches
        SET (MPI_INCLUDE_DIR ${MPI_INCLUDE_DIR} ${MPI_INCLUDE_DIR}/mpi2c++)
        SET (MPI_LIBRARIES pmpich++ ${MPI_LIBRARIES})
        SET (MPI_CCFLAGS ${MPI_CCFLAGS} -DHAVE_MPI_CPP)
    ENDIF (MPI_CPP_BINDING)
ENDIF (WIN32)