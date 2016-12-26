include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

if (BUILD_dborl_cluster)
    add_executable (dborl_shock_match 
             dborl_match_shock_main.cxx 
             dborl_match_shock_processor.h 
             dborl_match_shock_processor.cxx 
             dborl_cluster+dborl_match_shock_processor_input-dborl_match_shock_processor-float-.cxx)

    add_executable (dborl_shock_patch_extract 
             dborl_extract_patch_main.cxx 
             dborl_extract_patch_processor.h 
             dborl_extract_patch_processor.cxx 
             dborl_cluster+dborl_extract_patch_processor_input-dborl_extract_patch_processor-float-.cxx)

    add_executable (dborl_shock_patch_match 
             dborl_match_patch_main.cxx 
             dborl_match_patch_processor.h 
             dborl_match_patch_processor.cxx 
             dborl_cluster+dborl_match_patch_processor_input-dborl_match_patch_processor-float-.cxx)

    add_executable (dborl_shock_patch_detect 
             dborl_detect_patch_main.cxx 
             dborl_detect_patch_processor.h 
             dborl_detect_patch_processor.cxx 
             dborl_cluster+dborl_detect_patch_processor_input-dborl_detect_patch_processor-char-.cxx)

    add_executable (dborl_patch_extract
             dborl_patch_extract_params.h 
             dborl_patch_extract_params_sptr.h 
             dborl_patch_extract_main.cxx 
             dborl_patch_extract.h 
             dborl_patch_extract.cxx 
             dborl_cluster+dborl_patch_extract_input-dborl_patch_extract-float-.cxx
             vbl_smart_ptr+dborl_patch_extract_params-.cxx)

    add_executable (dborl_patch_match
             dborl_patch_match_params.h 
             dborl_patch_match_params_sptr.h 
             dborl_patch_match_main.cxx 
             dborl_patch_match.h 
             dborl_patch_match.cxx 
             dborl_cluster+dborl_patch_match_input-dborl_patch_match-float-.cxx
             vbl_smart_ptr+dborl_patch_match_params-.cxx)

    include_directories (${MPI_INCLUDE_DIR})
    if (MPI_CCFLAGS)
        add_definitions (${MPI_CCFLAGS})
    endif (MPI_CCFLAGS)
    link_directories (${MPI_LIBRARY_DIR})
    target_link_libraries (dborl_shock_match dborl_mpi dborl_algo dbskr dbskr_algoio ${MPI_LIBRARIES})
    target_link_libraries (dborl_shock_patch_extract dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr dbskr_algoio ${MPI_LIBRARIES})
    target_link_libraries (dborl_shock_patch_match dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr dbskr_algoio ${MPI_LIBRARIES})
    target_link_libraries (dborl_shock_patch_detect dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr dbskr_algoio ${MPI_LIBRARIES})
    target_link_libraries (dborl_patch_extract dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr dbskr_algoio ${MPI_LIBRARIES})
    target_link_libraries (dborl_patch_match dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr dbskr_algoio ${MPI_LIBRARIES})

    if (MPI_LINKFLAGS)
        set_target_properties (dborl_shock_match PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_shock_patch_extract PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_shock_patch_match PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_shock_patch_detect PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_patch_extract PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_patch_match PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    endif (MPI_LINKFLAGS)
    
endif(BUILD_dborl_cluster)


set (dborl_shock_match_no_mpi_src
     dborl_match_shock_main_no_mpi.cxx 
     dborl_match_shock_processor.h 
     dborl_match_shock_processor.cxx 
    )  
add_executable (dborl_shock_match_no_mpi ${dborl_shock_match_no_mpi_src})
target_link_libraries (dborl_shock_match_no_mpi dborl_algo vil dbdet dbsk2d_algo dbskr_algo dbskr dbskr_algoio)

