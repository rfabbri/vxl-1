include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas/bsta)
include_directories( ${lems_SOURCE_DIR}/contrib/bvam )
include_directories( ${lems_SOURCE_DIR}/db3p)

set(bvam_sources

    bvam_memory_chunk.h             bvam_memory_chunk.cxx 
    bvam_mog_grey_processor.h       bvam_mog_grey_processor.cxx  
    bvam_mog_rgb_processor.h        bvam_mog_rgb_processor.cxx
    bvam_world_params.h             bvam_world_params.cxx
    bvam_voxel_slab_base.h
    bvam_voxel_slab.h               bvam_voxel_slab.txx
    bvam_voxel_storage.h           
    bvam_voxel_storage_disk.h       bvam_voxel_storage_disk.txx
    bvam_voxel_storage_mem.h        bvam_voxel_storage_mem.txx
    bvam_voxel_slab_iterator.h      bvam_voxel_slab_iterator.txx
    bvam_voxel_grid_base.h          
    bvam_voxel_grid.h               bvam_voxel_grid.txx
    bvam_voxel_traits.h
    bvam_voxel_world.h              bvam_voxel_world.cxx
    bvam_image_metadata.h           bvam_image_metadata.cxx
    bvam_util.h                     bvam_util.cxx
)

aux_source_directory(Templates bvam_sources)

add_library( bvam ${bvam_sources})


target_link_libraries( bvam bsta bsta_algo vcl vgl vgl_io vnl vil vgl_algo vil_algo vpgl vul vbl vsl bgeo)


subdirs( io )
subdirs( apps )
subdirs( pro )
subdirs( algo )


if( BUILD_TESTING )
  subdirs(tests)
endif( BUILD_TESTING )



