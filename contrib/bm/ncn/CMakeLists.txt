# This is contrib/bm/ncn/CMakeLists.txt
# Non-Comact Neighborhood


project( ncn )
include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg ) #bsta is the next step down

set( ncn_sources
     ncn_factory.h                    ncn_factory.cxx
     ncn_neighborhood.h               ncn_neighborhood.cxx
     ncn_builder.h                    ncn_builder.cxx
     ncn_neighborhood_sptr.h          
     smart_ptr+ncn_neighborhood-.cxx
     ncn_vgl_point_2d_less_than.h
     ncn_visualization.h              ncn_visualization.cxx)
     
add_library( ncn ${ncn_sources} )
target_link_libraries( ncn vil vidl vul vsl vgl vbl bsta )

if( BUILD_TESTING )
  subdirs( tests )
endif( BUILD_TESTING )

subdirs( executables )