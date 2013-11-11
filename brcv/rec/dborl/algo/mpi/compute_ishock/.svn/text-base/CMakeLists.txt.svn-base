INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

IF (BUILD_dborl_cluster)
    ADD_EXECUTABLE (mpi_compute_ishock
       mpi_compute_ishock_processor.h
       mpi_compute_ishock_processor.cxx
       mpi_compute_ishock_main.cxx
       mpi_compute_ishock_params.h
       mpi_compute_ishock_params.cxx
       dborl_cluster+mpi_compute_ishock_processor_input-mpi_compute_ishock_processor-float-.cxx
    )

    INCLUDE_DIRECTORIES (${MPI_INCLUDE_DIR})
    
    IF (MPI_CCFLAGS)
        ADD_DEFINITIONS (${MPI_CCFLAGS})
    ENDIF (MPI_CCFLAGS)
    
    LINK_DIRECTORIES (${MPI_LIBRARY_DIR})
    
    TARGET_LINK_LIBRARIES (mpi_compute_ishock dborl_mpi dborl_algo bxml dbdet_pro dbsk2d_pro ${MPI_LIBRARIES})

    IF (MPI_LINKFLAGS)     
        SET_TARGET_PROPERTIES (mpi_compute_ishock PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    ENDIF (MPI_LINKFLAGS)
    
ENDIF(BUILD_dborl_cluster)


