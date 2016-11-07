# This is /contrib/bm/non_compact_neighborhood_experiments

include_directories( ${dbcl_SOURCE_DIR} )
#message( ${dbcl_SOURCE_DIR} )

include_directories( ${dncn_SOURCE_DIR} )
#message( ${dncn_SOURCE_DIR} )

include_directories( ${dvrml_SOURCE_DIR} )

add_executable( westin_classification1 westin_classification1.cxx )
target_link_libraries( westin_classification1 dncn dbcl )

add_executable( street_webcam	street_webcam.cxx )
target_link_libraries( street_webcam dncn dbcl )

add_executable( state_machine_classifier_executable state_machine_classifier_executable.cxx )
target_link_libraries( state_machine_classifier_executable dncn dbcl )

add_executable( westin_state_machine_classifier westin_state_machine_classifier.cxx )
target_link_libraries( westin_state_machine_classifier dncn dbcl )