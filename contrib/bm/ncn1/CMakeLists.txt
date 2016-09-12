# This is contrib/bm/ncn1/CMakeLists.txt
# Non-Compact Neighborhood



project( ncn1 )
include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg ) #bsta is the next step down
#to parse xml

set( ncn1_sources
	 
	 ncn1_neighborhood.h          				ncn1_neighborhood.cxx
	 ncn1_neighborhood_sptr.h
	 smart_ptr+ncn1_neighborhood-.cxx
	 ncn1_feature.h				  				ncn1_feature.cxx
	 ncn1_feature_sptr.h
	 smart_ptr+ncn1_feature-.cxx
	 #ncn1_classifier_factory.h	  				ncn1_classifier_factory.cxx
	 #ncn1_classifier.h
	 #ncn1_classifier_sptr.h
	 #smart_ptr+ncn1_classifier-.cxx
	 #ncn1_linear_classifier.h	  				ncn1_linear_classifier.cxx
	 #ncn1_linear_classifier_training_data.h		ncn1_linear_classifier_training_data.cxx
	 #ncn1_linear_classifier_training_point.h	ncn1_linear_classifier_training_point.cxx
	 #ncn1_linear_classifier_io.h				ncn1_linear_classifier_io.cxx
	 
	 #ncn1_linear_classifier_definitions.h
	 ncn1_vgl_point_2d_less_than.h
	 ncn1_factory.h               ncn1_factory.cxx
	 ncn1_utilities.h			  ncn1_utilities.cxx )
	 
add_library( ncn1 ${ncn1_sources} )
target_link_libraries( ncn1 vil vil_io vidl vul vsl vgl vbl bsta)

if( BUILD_TESTING )
  subdirs( tests )
endif( BUILD_TESTING )

subdirs( executables )

