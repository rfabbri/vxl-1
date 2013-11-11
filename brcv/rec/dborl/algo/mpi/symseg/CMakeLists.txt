INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

IF (BUILD_dborl_cluster)
    ADD_EXECUTABLE (dborl_sks_match_shape 
       dborl_sks_match_shape_processor.h
       dborl_sks_match_shape_processor.cxx
       dborl_sks_match_shape_main.cxx
       dborl_sks_match_shape_params.h
       dborl_sks_match_shape_params.cxx
       dborl_cluster+dborl_sks_match_shape_processor_input-dborl_sks_match_shape_processor-float-.cxx
       bpro1_xio_parameters.h
       bpro1_xio_parameters.cxx
    )

    INCLUDE_DIRECTORIES (${MPI_INCLUDE_DIR})
    
    IF (MPI_CCFLAGS)
        ADD_DEFINITIONS (${MPI_CCFLAGS})
    ENDIF (MPI_CCFLAGS)
    
    LINK_DIRECTORIES (${MPI_LIBRARY_DIR})
    
    TARGET_LINK_LIBRARIES (dborl_sks_match_shape dborl_mpi dborl_algo bxml dbsks_pro dbsksp_xio ${MPI_LIBRARIES})

    IF (MPI_LINKFLAGS)     
        SET_TARGET_PROPERTIES (dborl_sks_match_shape PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    ENDIF (MPI_LINKFLAGS)
    
ENDIF(BUILD_dborl_cluster)


