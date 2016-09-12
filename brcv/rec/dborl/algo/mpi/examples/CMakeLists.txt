include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

if (BUILD_dborl_cluster)
    add_executable (dborl_example_processor example_main.cxx example_processor.h example_processor.cxx dborl_cluster+int-example_processor-float-.cxx)
    add_executable (dborl_example_processor2 example_main2.cxx example_processor2.h example_processor2.cxx dborl_cluster+int-example_processor2-ex_class-.cxx)
    add_executable (dborl_example_processor3 example_main3.cxx example_processor3.h example_processor3.cxx dborl_cluster+int-example_processor3-ex_class3-.cxx)

    set (dborl_extract_contour_src
     dborl_extract_contour_main.cxx 
     dborl_extract_contour.h dborl_extract_contour.cxx 
     dborl_extract_contour_params.h 
     dborl_extract_contour_params_sptr.h
     vbl_smart_ptr+dborl_extract_contour_params-.cxx
     dborl_cluster+int-dborl_extract_contour-int-.cxx
    )  
    add_executable (dborl_extract_contour ${dborl_extract_contour_src})

    set (dborl_extract_shock_src
     dborl_extract_shock_main.cxx 
     dborl_extract_shock.h dborl_extract_shock.cxx 
     dborl_extract_shock_params.h 
     dborl_extract_shock_params_sptr.h
     vbl_smart_ptr+dborl_extract_shock_params-.cxx
     dborl_cluster+int-dborl_extract_shock-int-.cxx
    )  
    add_executable (dborl_extract_shock ${dborl_extract_shock_src})

#    set (dborl_categorization_src
#     dborl_categorization_main.cxx 
#     dborl_categorization.h dborl_categorization.cxx 
#     dborl_categorization_params.h 
#     dborl_categorization_params_sptr.h
#     vbl_smart_ptr+dborl_categorization_params-.cxx
#     dborl_cluster+int-dborl_categorization-int-.cxx
#    )  
#    add_executable (dborl_categorization ${dborl_categorization_src})

    set (dborl_categorization_simple_src
     dborl_categorization_simple_main.cxx 
     dborl_categorization_simple.h dborl_categorization_simple.cxx 
     dborl_categorization_simple_params.h 
     dborl_categorization_simple_params_sptr.h
     vbl_smart_ptr+dborl_categorization_simple_params-.cxx
     dborl_cluster+int-dborl_categorization_simple-int-.cxx
    )  
    add_executable (dborl_categorization_simple ${dborl_categorization_simple_src})

    include_directories (${MPI_INCLUDE_DIR})
    if (MPI_CCFLAGS)
        add_definitions (${MPI_CCFLAGS})
    endif (MPI_CCFLAGS)
    link_directories (${MPI_LIBRARY_DIR})
    target_link_libraries (dborl_example_processor dborl_mpi dborl_algo ${MPI_LIBRARIES})
    target_link_libraries (dborl_example_processor2 dborl_mpi dborl_algo ${MPI_LIBRARIES})
    target_link_libraries (dborl_example_processor3 dborl_mpi dborl_algo ${MPI_LIBRARIES})
    target_link_libraries (dborl_extract_contour dborl_mpi dborl_algo vil dbdet ${MPI_LIBRARIES})
    target_link_libraries (dborl_extract_shock dbskr_algo dbskr dborl_mpi dborl_algo vil dbdet  dbsk2d_algo ${MPI_LIBRARIES})
#    target_link_libraries (dborl_categorization dborl_mpi dborl_algo vil dbdet dbskr ${MPI_LIBRARIES})
    target_link_libraries (dborl_categorization_simple dborl_mpi dborl_algo vil dbdet dbskr ${MPI_LIBRARIES})

    if (MPI_LINKFLAGS)
        set_target_properties (dborl_example_processor PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_example_processor2 PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_example_processor3 PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_extract_contour PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_extract_shock PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
#        set_target_properties (dborl_categorization PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
        set_target_properties (dborl_categorization_simple PROPERTIES LINK_FLAGS ${MPI_LINKFLAGS})
    endif (MPI_LINKFLAGS)
    
endif(BUILD_dborl_cluster)

set (dborl_categorization_evaluate_src
     dborl_categorization_evaluate_main.cxx 
     dborl_categorization_evaluate.h dborl_categorization_evaluate.cxx 
     dborl_categorization_evaluate_params.h 
     dborl_categorization_evaluate_params_sptr.h
     vbl_smart_ptr+dborl_categorization_evaluate_params-.cxx
    )  
add_executable (dborl_categorization_evaluate ${dborl_categorization_evaluate_src})
target_link_libraries (dborl_categorization_evaluate dborl_algo)

set (dborl_extract_shock_no_mpi_src
     dborl_extract_shock_main_no_mpi.cxx 
     dborl_extract_shock.h dborl_extract_shock.cxx 
     dborl_extract_shock_params.h 
     dborl_extract_shock_params_sptr.h
     vbl_smart_ptr+dborl_extract_shock_params-.cxx
    )  
add_executable (dborl_extract_shock_no_mpi ${dborl_extract_shock_no_mpi_src})
target_link_libraries (dborl_extract_shock_no_mpi dborl_algo vil dbdet dbskr_algo dbskr dbsk2d_algo )



