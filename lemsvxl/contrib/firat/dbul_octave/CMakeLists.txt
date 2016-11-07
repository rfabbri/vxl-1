include_directories( ${VXL_BRL_INCLUDE_DIR} )
if(OCTAVE_FOUND)        
       
    set(dbul_octave_test_sources
       dbul_octave_test.cxx
    )
    
    add_executable (dbul_octave_test ${dbul_octave_test_sources})
    target_link_libraries (dbul_octave_test dbul) 
    
     add_executable (dbul_octave_test2 test.cxx)
    target_link_libraries (dbul_octave_test2 dbul)  
endif(OCTAVE_FOUND)

