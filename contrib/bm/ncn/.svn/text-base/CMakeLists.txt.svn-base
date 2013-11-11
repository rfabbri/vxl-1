# This is contrib/bm/ncn/CMakeLists.txt
# Non-Comact Neighborhood


project( ncn )
INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg ) #bsta is the next step down

SET( ncn_sources
     ncn_factory.h                    ncn_factory.cxx
     ncn_neighborhood.h               ncn_neighborhood.cxx
     ncn_builder.h                    ncn_builder.cxx
     ncn_neighborhood_sptr.h          
     smart_ptr+ncn_neighborhood-.cxx
     ncn_vgl_point_2d_less_than.h
     ncn_visualization.h              ncn_visualization.cxx)
     
ADD_LIBRARY( ncn ${ncn_sources} )
TARGET_LINK_LIBRARIES( ncn vil vidl vul vsl vgl vbl bsta )

IF( BUILD_TESTING )
  SUBDIRS( tests )
ENDIF( BUILD_TESTING )

SUBDIRS( executables )