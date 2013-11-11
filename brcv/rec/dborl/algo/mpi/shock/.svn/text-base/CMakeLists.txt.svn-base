INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

IF (BUILD_dborl_cluster)
    ADD_EXECUTABLE (dborl_shock_match 
             dborl_match_shock_main.cxx 
             dborl_match_shock_processor.h 
             dborl_match_shock_processor.cxx 
             dborl_cluster+dborl_match_shock_processor_input-dborl_match_shock_processor-float-.cxx)

    ADD_EXECUTABLE (dborl_shock_patch_extract 
             dborl_extract_patch_main.cxx 
             dborl_extract_patch_processor.h 
             dborl_extract_patch_processor.cxx 
             dborl_cluster+dborl_extract_patch_processor_input-dborl_extract_patch_processor-float-.cxx)

    ADD_EXECUTABLE (dborl_shock_patch_match 
             dborl_match_patch_main.cxx 
             dborl_match_patch_processor.h 
             dborl_match_patch_processor.cxx 
             dborl_cluster+dborl_match_patch_processor_input-dborl_match_patch_processor-float-.cxx)

    ADD_EXECUTABLE (dborl_shock_patch_detect 
             dborl_detect_patch_main.cxx 
             dborl_detect_patch_processor.h 
             dborl_detect_patch_processor.cxx 
             dborl_cluster+dborl_detect_patch_processor_input-dborl_detect_patch_processor-char-.cxx)

    ADD_EXECUTABLE (dborl_patch_extract
             dborl_patch_extract_params.h 
             dborl_patch_extract_params_sptr.h 
             dborl_patch_extract_main.cxx 
             dborl_patch_extract.h 
             dborl_patch_extract.cxx 
             dborl_cluster+dborl_patch_extract_input-dborl_patch_extract-float-.cxx
             vbl_smart_ptr+dborl_patch_extract_params-.cxx)

    ADD_EXECUTABLE (dborl_patch_match
             dborl_patch_match_params.h 
             dborl_patch_match_params_sptr.h 
             dborl_patch_match_main.cxx 
             dborl_patch_match.h 
             dborl_patch_match.cxx 
             dborl_cluster+dborl_patch_match_input-dborl_patch_match-float-.cxx
             vbl_smart_ptr+dborl_patch_match_params-.cxx)

    INCLUDE_DIRECTORIES (${MPI_INCLUDE_DIR})
    IF (MPI_CCFLAGS)
        ADD_DEFINITIONS (${MPI_CCFLAGS})
    ENDIF (MPI_CCFLAGS)
    LINK_DIRECTORIES (${MPI_LIBRARY_DIR})
    TARGET_LINK_LIBRARIES (dborl_shock_match dborl_mpi dborl_algo dbskr dbskr_algoio ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_shock_patch_extract dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr dbskr_algoio ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_shock_patch_match dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr dbskr_algoio ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_shock_patch_detect dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr dbskr_algoio ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_patch_extract dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr dbskr_algoio ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_patch_match dborl_mpi dborl_algo dbskr_pro dbskr_algo dbskr dbskr_algoio ${MPI_LIBRARIES})

    IF (MPI_LINKFLAGS)
        SET_TARGET_PROPERTIES (dborl_shock_match PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_shock_patch_extract PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_shock_patch_match PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_shock_patch_detect PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_patch_extract PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_patch_match PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    ENDIF (MPI_LINKFLAGS)
    
ENDIF(BUILD_dborl_cluster)


SET (dborl_shock_match_no_mpi_src
     dborl_match_shock_main_no_mpi.cxx 
     dborl_match_shock_processor.h 
     dborl_match_shock_processor.cxx 
    )  
ADD_EXECUTABLE (dborl_shock_match_no_mpi ${dborl_shock_match_no_mpi_src})
TARGET_LINK_LIBRARIES (dborl_shock_match_no_mpi dborl_algo vil dbdet dbsk2d_algo dbskr_algo dbskr dbskr_algoio)

