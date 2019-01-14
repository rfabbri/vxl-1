include_directories( ${LEMS_SOURCE_DIR}/db3p )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )

if (BUILD_dborl_cluster)
    add_executable (mpi_compute_ishock
       mpi_compute_ishock_processor.h
       mpi_compute_ishock_processor.cxx
       mpi_compute_ishock_main.cxx
       mpi_compute_ishock_params.h
       mpi_compute_ishock_params.cxx
       dborl_cluster+mpi_compute_ishock_processor_input-mpi_compute_ishock_processor-float-.cxx
    )

    include_directories (${MPI_INCLUDE_DIR})
    
    if (MPI_CCFLAGS)
        add_definitions (${MPI_CCFLAGS})
    endif (MPI_CCFLAGS)
    
    link_directories (${MPI_LIBRARY_DIR})
    
    target_link_libraries (mpi_compute_ishock dborl_mpi dborl_algo bxml dbdet_pro dbsk2d_pro ${MPI_LIBRARIES})

    if (MPI_LINKFLAGS)     
        set_target_properties (mpi_compute_ishock PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    endif (MPI_LINKFLAGS)
    
endif(BUILD_dborl_cluster)


