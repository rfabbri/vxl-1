INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
IF(OCTAVE_FOUND)        
       
    SET(dbul_octave_test_sources
       dbul_octave_test.cxx
    )
    
    ADD_EXECUTABLE (dbul_octave_test ${dbul_octave_test_sources})
    TARGET_LINK_LIBRARIES (dbul_octave_test dbul) 
    
     ADD_EXECUTABLE (dbul_octave_test2 test.cxx)
    TARGET_LINK_LIBRARIES (dbul_octave_test2 dbul)  
ENDIF(OCTAVE_FOUND)

