include_directories( ${LEMS_SOURCE_DIR}/db3p )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )

if (BUILD_dborl_cluster)
    add_executable (dborl_sks_match_shape 
       dborl_sks_match_shape_processor.h
       dborl_sks_match_shape_processor.cxx
       dborl_sks_match_shape_main.cxx
       dborl_sks_match_shape_params.h
       dborl_sks_match_shape_params.cxx
       dborl_cluster+dborl_sks_match_shape_processor_input-dborl_sks_match_shape_processor-float-.cxx
       bpro1_xio_parameters.h
       bpro1_xio_parameters.cxx
    )

    include_directories (${MPI_INCLUDE_DIR})
    
    if (MPI_CCFLAGS)
        add_definitions (${MPI_CCFLAGS})
    endif (MPI_CCFLAGS)
    
    link_directories (${MPI_LIBRARY_DIR})
    
    target_link_libraries (dborl_sks_match_shape dborl_mpi dborl_algo bxml dbsks_pro dbsksp_xio ${MPI_LIBRARIES})

    if (MPI_LINKFLAGS)     
        set_target_properties (dborl_sks_match_shape PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    endif (MPI_LINKFLAGS)
    
endif(BUILD_dborl_cluster)


