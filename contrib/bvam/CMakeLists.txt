INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas/bsta)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/bvam )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p)

SET(bvam_sources

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

AUX_SOURCE_DIRECTORY(Templates bvam_sources)

ADD_LIBRARY( bvam ${bvam_sources})


TARGET_LINK_LIBRARIES( bvam bsta bsta_algo vcl vgl vgl_io vnl vil vgl_algo vil_algo vpgl vul vbl vsl bgeo)


SUBDIRS( io )
SUBDIRS( apps )
SUBDIRS( pro )
SUBDIRS( algo )


IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )



