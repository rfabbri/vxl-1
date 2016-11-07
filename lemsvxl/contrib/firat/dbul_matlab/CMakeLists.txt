set(dbul_matlab_test_sources
       dbul_matlab_test.cxx
    )
    
    add_executable (dbul_matlab_test ${dbul_matlab_test_sources})
    target_link_libraries (dbul_matlab_test dbul vnl)