include_directories(${VXL_GEL_INCLUDE_DIR})

set(geom_src
             geom_probe_volume.h                    
             geom_probe_volume_sptr.h
             geom_volume.h                          geom_volume.cxx
             geom_volume_sptr.h
             geom_index_structure.h                 geom_index_structure.cxx
             geom_rectangular_probe_volume.h        geom_rectangular_probe_volume.cxx
             geom_voxel_enumerator.h
             geom_voxel_enumerator_sptr.h
             geom_voxel_enumerator_from_box.h       geom_voxel_enumerator_from_box.cxx
)

aux_source_directory(Templates geom_src)

add_library(geom ${geom_src})
target_link_libraries(geom vbl vgl vsol biob)

subdirs(examples)

if(BUILD_TESTING)
  subdirs( tests )
endif(BUILD_TESTING)

