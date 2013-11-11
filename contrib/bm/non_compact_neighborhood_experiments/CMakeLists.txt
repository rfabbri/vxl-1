# This is /contrib/bm/non_compact_neighborhood_experiments

INCLUDE_DIRECTORIES( ${dbcl_SOURCE_DIR} )
#MESSAGE( ${dbcl_SOURCE_DIR} )

INCLUDE_DIRECTORIES( ${dncn_SOURCE_DIR} )
#MESSAGE( ${dncn_SOURCE_DIR} )

INCLUDE_DIRECTORIES( ${dvrml_SOURCE_DIR} )

ADD_EXECUTABLE( westin_classification1 westin_classification1.cxx )
TARGET_LINK_LIBRARIES( westin_classification1 dncn dbcl )

ADD_EXECUTABLE( street_webcam	street_webcam.cxx )
TARGET_LINK_LIBRARIES( street_webcam dncn dbcl )

ADD_EXECUTABLE( state_machine_classifier_executable state_machine_classifier_executable.cxx )
TARGET_LINK_LIBRARIES( state_machine_classifier_executable dncn dbcl )

ADD_EXECUTABLE( westin_state_machine_classifier westin_state_machine_classifier.cxx )
TARGET_LINK_LIBRARIES( westin_state_machine_classifier dncn dbcl )