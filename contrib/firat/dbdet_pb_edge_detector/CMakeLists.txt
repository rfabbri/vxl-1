INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/seg)

IF(OCTAVE_FOUND)        
       
    SET(dbdet_pb_edge_detector_test_sources
       dbdet_pb_edge_detector_test.cxx
    )   
    
    ADD_EXECUTABLE (dbdet_pb_edge_detector_test ${dbdet_pb_edge_detector_test_sources})
    TARGET_LINK_LIBRARIES (dbdet_pb_edge_detector_test dbdet_algo)
ENDIF(OCTAVE_FOUND)


