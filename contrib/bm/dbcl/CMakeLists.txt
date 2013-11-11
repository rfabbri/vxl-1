# This is contrib/bm/dbcl/CMakeLists.txt
# Dev-Brown Classifier Library
# Library of statistical classifiers implemented using factory method.

IF(VXL_EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )
	#MESSAGE("expat found")
	PROJECT( dbcl )
	INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
	INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg ) #bsta is one step down
	
	#to include non-compact neighborhood structures
	INCLUDE_DIRECTORIES( ${dncn_SOURCE_DIR} )
	
	#to include dvrml api
	INCLUDE_DIRECTORIES( ${dvrml_SOURCE_DIR} )
	#MESSAGE("dvrml_SOURCE_DIR = " ${dvrml_SOURCE_DIR} )
	
	#to parse xml
	INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIRS} )
	INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )
	
	SET( dbcl_sources
		 dbcl_classifier.h						                  dbcl_classifier.cxx
		 dbcl_classifier_sptr.h
		 smart_ptr+dbcl_classifier-.cxx
		 dbcl_temporal_feature.h
		 dbcl_temporal_feature_sptr.h
		 smart_ptr+dbcl_temporal_feature-.cxx
		 dbcl_classifier_factory.h				              dbcl_classifier_factory.cxx
		 dbcl_image_classifier.h				                dbcl_image_classifier.cxx
		 dbcl_image_classifier_sptr.h
		 smart_ptr+dbcl_image_classifier-.cxx
		 dbcl_classifier_io.h					                  dbcl_classifier_io.cxx
		 dbcl_classifier_io_defs.h
		 dbcl_linear_classifier.h				                dbcl_linear_classifier.cxx
		 dbcl_linear_classifier_sptr.h
		 smart_ptr+dbcl_linear_classifier-.cxx 
		 dbcl_linear_classifier_vrml_view.h		          dbcl_linear_classifier_vrml_view.cxx
		 dbcl_state_machine_classifier.h		            dbcl_state_machine_classifier.cxx
		 dbcl_state_machine_classifier_sptr.h
		 smart_ptr+dbcl_state_machine_classifier-.cxx
		 dbcl_state_machine_node.h				              dbcl_state_machine_node.cxx
		 dbcl_state_machine_node_sptr.h
		 smart_ptr+dbcl_state_machine_node-.cxx
		 dbcl_state_machine_frame_clock.h		            dbcl_state_machine_frame_clock.cxx
		 dbcl_state_machine_frame_clock_sptr.h
		 smart_ptr+dbcl_state_machine_frame_clock-.cxx
		 dbcl_state_machine_view.h				              dbcl_state_machine_view.cxx
     dbcl_state_machine_image_classifier.h          dbcl_state_machine_image_classifier.cxx
     dbcl_ncn_extractor.h                           dbcl_ncn_extractor.cxx
     dbcl_ncn_extractor_sptr.h
     smart_ptr+dbcl_ncn_extractor-.cxx
     dbcl_sift_extractor.h                          dbcl_sift_extractor.cxx
     dbcl_sift_extractor_sptr.h
     smart_ptr+dbcl_sift_extractor-.cxx
     dbcl_vgl_point_2d_dist_compare.h
		 )

	ADD_LIBRARY( dbcl ${dbcl_sources} )
	TARGET_LINK_LIBRARIES( dbcl dncn vul vbl bsta bsta_algo dvrml ${EXPAT_LIBRARIES} expatpp )

	SUBDIRS( tests )
	
ENDIF(VXL_EXPAT_FOUND)