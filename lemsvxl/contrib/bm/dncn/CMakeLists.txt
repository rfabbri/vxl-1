# This is contrib/bm/dncn/CMakeLists.txt

project( dncn )

#message( "dncn_SOURCE_DIR = " ${dncn_SOURCE_DIR} )
#set( DNCN_INCLUDE_DIR ${VXL_BRL_INCLUDE_DIR} )

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg ) #bsta is one step down

set ( dncn_sources	
	  dncn_factory.h						    dncn_factory.cxx
	  dncn_target_list_2d.h					    dncn_target_list_2d.cxx
	  dncn_target_list_2d_sptr.h
	  smart_ptr+dncn_target_list_2d-.cxx
	  dncn_target_2d.h						    dncn_target_2d.cxx
	  dncn_target_2d_sptr.h
	  smart_ptr+dncn_target_2d-.cxx
	  dncn_neighborhood_2d.h					dncn_neighborhood_2d.cxx
	  dncn_neighborhood_2d_sptr.h
	  smart_ptr+dncn_neighborhood_2d-.cxx
	  dncn_feature.h						    dncn_feature.h
	  dncn_feature_sptr.h
	  dncn_image_point_2d.h
	  smart_ptr+dncn_feature-.cxx
	  dncn_less_than.h
	  )
add_library( dncn ${dncn_sources} )

target_link_libraries( dncn vul bsta vidl vsl vil_io bapl )

add_subdirectory( tests )

