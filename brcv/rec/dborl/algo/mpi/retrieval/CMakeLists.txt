INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

IF (BUILD_dborl_cluster)

    ADD_EXECUTABLE (dborl_shock_retrieval 
             dborl_shock_retrieval_params.h 
             dborl_shock_retrieval_params_sptr.h 
             dborl_shock_retrieval_main.cxx 
             dborl_shock_retrieval.h 
             dborl_shock_retrieval.cxx 
             dborl_cluster+dborl_shock_retrieval_input-dborl_shock_retrieval-float-.cxx
             vbl_smart_ptr+dborl_shock_retrieval_params-.cxx)

    ADD_EXECUTABLE (dborl_curve_retrieval 
             dborl_curve_retrieval_params.h 
             dborl_curve_retrieval_params_sptr.h 
             dborl_curve_retrieval_main.cxx 
             dborl_curve_retrieval.h 
             dborl_curve_retrieval.cxx 
             dborl_cluster+dborl_curve_retrieval_input-dborl_curve_retrieval-float-.cxx
             vbl_smart_ptr+dborl_curve_retrieval_params-.cxx)


    INCLUDE_DIRECTORIES (${MPI_INCLUDE_DIR})
    IF (MPI_CCFLAGS)
        ADD_DEFINITIONS (${MPI_CCFLAGS})
    ENDIF (MPI_CCFLAGS)
    LINK_DIRECTORIES (${MPI_LIBRARY_DIR})
    TARGET_LINK_LIBRARIES (dborl_shock_retrieval dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr_algoio ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_curve_retrieval dborl_mpi dborl_algo dbcvr_pro ${MPI_LIBRARIES})

    IF (MPI_LINKFLAGS)
        SET_TARGET_PROPERTIES (dborl_shock_retrieval PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_curve_retrieval PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    ENDIF (MPI_LINKFLAGS)
    
ENDIF(BUILD_dborl_cluster)
