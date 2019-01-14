# contrib/bvam/io/CMakeLists.txt

##### old CMakeLists.txt ######
#include_directories( ${LEMS_SOURCE_DIR}/db3p)

#set(bvam_io_sources
#   bvam_io_config_parser.cxx bvam_io_config_parser.h
#   bvam_io_structs.h    
#)

#add_library(bvam_io ${bvam_io_sources})

#if ( APPLE )
#  target_link_libraries(bvam_io vgl vsl vbl expatpp expat)
#else( APPLE )
#  target_link_libraries(bvam_io vgl vsl vbl expatpp)
#endif ( APPLE )
##############################

set(bvam_io_sources
    bvam_io_voxel_world.h  bvam_io_voxel_world.cxx
    bvam_io_world_params.h bvam_io_world_params.cxx
)

aux_source_directory(Templates bvam_io_sources)

add_library(bvam_io ${bvam_io_sources})
target_link_libraries(bvam_io bvam vsl vbl vil vil_io)


