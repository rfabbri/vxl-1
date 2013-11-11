# contrib/bvam/io/CMakeLists.txt

##### old CMakeLists.txt ######
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p)

#SET(bvam_io_sources
#   bvam_io_config_parser.cxx bvam_io_config_parser.h
#   bvam_io_structs.h    
#)

#ADD_LIBRARY(bvam_io ${bvam_io_sources})

#IF ( APPLE )
#  TARGET_LINK_LIBRARIES(bvam_io vgl vsl vbl expatpp expat)
#ELSE( APPLE )
#  TARGET_LINK_LIBRARIES(bvam_io vgl vsl vbl expatpp)
#ENDIF ( APPLE )
##############################

SET(bvam_io_sources
    bvam_io_voxel_world.h  bvam_io_voxel_world.cxx
    bvam_io_world_params.h bvam_io_world_params.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvam_io_sources)

ADD_LIBRARY(bvam_io ${bvam_io_sources})
TARGET_LINK_LIBRARIES(bvam_io bvam vsl vbl vil vil_io)


