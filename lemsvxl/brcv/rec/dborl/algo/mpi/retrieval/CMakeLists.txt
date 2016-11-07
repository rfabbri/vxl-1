include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

if (BUILD_dborl_cluster)

    add_executable (dborl_shock_retrieval 
             dborl_shock_retrieval_params.h 
             dborl_shock_retrieval_params_sptr.h 
             dborl_shock_retrieval_main.cxx 
             dborl_shock_retrieval.h 
             dborl_shock_retrieval.cxx 
             dborl_cluster+dborl_shock_retrieval_input-dborl_shock_retrieval-float-.cxx
             vbl_smart_ptr+dborl_shock_retrieval_params-.cxx)

    add_executable (dborl_curve_retrieval 
             dborl_curve_retrieval_params.h 
             dborl_curve_retrieval_params_sptr.h 
             dborl_curve_retrieval_main.cxx 
             dborl_curve_retrieval.h 
             dborl_curve_retrieval.cxx 
             dborl_cluster+dborl_curve_retrieval_input-dborl_curve_retrieval-float-.cxx
             vbl_smart_ptr+dborl_curve_retrieval_params-.cxx)


    include_directories (${MPI_INCLUDE_DIR})
    if (MPI_CCFLAGS)
        add_definitions (${MPI_CCFLAGS})
    endif (MPI_CCFLAGS)
    link_directories (${MPI_LIBRARY_DIR})
    target_link_libraries (dborl_shock_retrieval dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr_algoio ${MPI_LIBRARIES})
    target_link_libraries (dborl_curve_retrieval dborl_mpi dborl_algo dbcvr_pro ${MPI_LIBRARIES})

    if (MPI_LINKFLAGS)
        set_target_properties (dborl_shock_retrieval PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_curve_retrieval PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    endif (MPI_LINKFLAGS)
    
endif(BUILD_dborl_cluster)
