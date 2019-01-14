include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories(${LEMS_SOURCE_DIR}/brcv/seg)

if(OCTAVE_FOUND)        
       
    set(dbdet_pb_edge_detector_test_sources
       dbdet_pb_edge_detector_test.cxx
    )   
    
    add_executable (dbdet_pb_edge_detector_test ${dbdet_pb_edge_detector_test_sources})
    target_link_libraries (dbdet_pb_edge_detector_test dbdet_algo)
endif(OCTAVE_FOUND)


