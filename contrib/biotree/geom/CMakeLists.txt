INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})

SET(geom_src
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

AUX_SOURCE_DIRECTORY(Templates geom_src)

ADD_LIBRARY(geom ${geom_src})
TARGET_LINK_LIBRARIES(geom vbl vgl vsol biob)

SUBDIRS(examples)

IF(BUILD_TESTING)
  SUBDIRS( tests )
ENDIF(BUILD_TESTING)

