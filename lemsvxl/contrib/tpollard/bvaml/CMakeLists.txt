include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/contrib/tpollard )

add_library( bvaml
             bvaml_params.h             bvaml_params.cxx
             bvaml_log_writer.h         bvaml_log_writer.cxx
             bvaml_world.h              bvaml_world.cxx
             bvaml_voxel.h              bvaml_voxel.cxx
             bvaml_supervoxel.h         bvaml_supervoxel.cxx
             bvaml_ray.h                bvaml_ray.cxx
             bvaml_world_viewer.h       bvaml_world_viewer.cxx
             vpgl_rays_to_voxels.h      vpgl_rays_to_voxels.cxx )


target_link_libraries( bvaml vcl vgl vnl vil vgl_algo vil_algo vpgl vul baml )
set( BVAML_INCLUDE_DIR ${bvaml_SOURCE_DIR} )

add_subdirectory( apps )
add_subdirectory( tests )
