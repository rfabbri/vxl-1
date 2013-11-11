INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

IF (BUILD_dborl_cluster)
    ADD_EXECUTABLE (dborl_example_processor example_main.cxx example_processor.h example_processor.cxx dborl_cluster+int-example_processor-float-.cxx)
    ADD_EXECUTABLE (dborl_example_processor2 example_main2.cxx example_processor2.h example_processor2.cxx dborl_cluster+int-example_processor2-ex_class-.cxx)
    ADD_EXECUTABLE (dborl_example_processor3 example_main3.cxx example_processor3.h example_processor3.cxx dborl_cluster+int-example_processor3-ex_class3-.cxx)

    SET (dborl_extract_contour_src
     dborl_extract_contour_main.cxx 
     dborl_extract_contour.h dborl_extract_contour.cxx 
     dborl_extract_contour_params.h 
     dborl_extract_contour_params_sptr.h
     vbl_smart_ptr+dborl_extract_contour_params-.cxx
     dborl_cluster+int-dborl_extract_contour-int-.cxx
    )  
    ADD_EXECUTABLE (dborl_extract_contour ${dborl_extract_contour_src})

    SET (dborl_extract_shock_src
     dborl_extract_shock_main.cxx 
     dborl_extract_shock.h dborl_extract_shock.cxx 
     dborl_extract_shock_params.h 
     dborl_extract_shock_params_sptr.h
     vbl_smart_ptr+dborl_extract_shock_params-.cxx
     dborl_cluster+int-dborl_extract_shock-int-.cxx
    )  
    ADD_EXECUTABLE (dborl_extract_shock ${dborl_extract_shock_src})

#    SET (dborl_categorization_src
#     dborl_categorization_main.cxx 
#     dborl_categorization.h dborl_categorization.cxx 
#     dborl_categorization_params.h 
#     dborl_categorization_params_sptr.h
#     vbl_smart_ptr+dborl_categorization_params-.cxx
#     dborl_cluster+int-dborl_categorization-int-.cxx
#    )  
#    ADD_EXECUTABLE (dborl_categorization ${dborl_categorization_src})

    SET (dborl_categorization_simple_src
     dborl_categorization_simple_main.cxx 
     dborl_categorization_simple.h dborl_categorization_simple.cxx 
     dborl_categorization_simple_params.h 
     dborl_categorization_simple_params_sptr.h
     vbl_smart_ptr+dborl_categorization_simple_params-.cxx
     dborl_cluster+int-dborl_categorization_simple-int-.cxx
    )  
    ADD_EXECUTABLE (dborl_categorization_simple ${dborl_categorization_simple_src})

    INCLUDE_DIRECTORIES (${MPI_INCLUDE_DIR})
    IF (MPI_CCFLAGS)
        ADD_DEFINITIONS (${MPI_CCFLAGS})
    ENDIF (MPI_CCFLAGS)
    LINK_DIRECTORIES (${MPI_LIBRARY_DIR})
    TARGET_LINK_LIBRARIES (dborl_example_processor dborl_mpi dborl_algo ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_example_processor2 dborl_mpi dborl_algo ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_example_processor3 dborl_mpi dborl_algo ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_extract_contour dborl_mpi dborl_algo vil dbdet ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_extract_shock dbskr_algo dbskr dborl_mpi dborl_algo vil dbdet  dbsk2d_algo ${MPI_LIBRARIES})
#    TARGET_LINK_LIBRARIES (dborl_categorization dborl_mpi dborl_algo vil dbdet dbskr ${MPI_LIBRARIES})
    TARGET_LINK_LIBRARIES (dborl_categorization_simple dborl_mpi dborl_algo vil dbdet dbskr ${MPI_LIBRARIES})

    IF (MPI_LINKFLAGS)
        SET_TARGET_PROPERTIES (dborl_example_processor PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_example_processor2 PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_example_processor3 PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_extract_contour PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_extract_shock PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
#        SET_TARGET_PROPERTIES (dborl_categorization PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        SET_TARGET_PROPERTIES (dborl_categorization_simple PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    ENDIF (MPI_LINKFLAGS)
    
ENDIF(BUILD_dborl_cluster)

SET (dborl_categorization_evaluate_src
     dborl_categorization_evaluate_main.cxx 
     dborl_categorization_evaluate.h dborl_categorization_evaluate.cxx 
     dborl_categorization_evaluate_params.h 
     dborl_categorization_evaluate_params_sptr.h
     vbl_smart_ptr+dborl_categorization_evaluate_params-.cxx
    )  
ADD_EXECUTABLE (dborl_categorization_evaluate ${dborl_categorization_evaluate_src})
TARGET_LINK_LIBRARIES (dborl_categorization_evaluate dborl_algo)

SET (dborl_extract_shock_no_mpi_src
     dborl_extract_shock_main_no_mpi.cxx 
     dborl_extract_shock.h dborl_extract_shock.cxx 
     dborl_extract_shock_params.h 
     dborl_extract_shock_params_sptr.h
     vbl_smart_ptr+dborl_extract_shock_params-.cxx
    )  
ADD_EXECUTABLE (dborl_extract_shock_no_mpi ${dborl_extract_shock_no_mpi_src})
TARGET_LINK_LIBRARIES (dborl_extract_shock_no_mpi dborl_algo vil dbdet dbskr_algo dbskr dbsk2d_algo )



