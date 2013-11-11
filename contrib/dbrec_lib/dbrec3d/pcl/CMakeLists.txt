# this is dbrec3d_pcl uses the point cloud library

FIND_PACKAGE( PCL 1.0 )

IF(PCL_FOUND)
	
  MESSAGE("PCL library found")
  add_definitions(${PCL_DEFINITIONS})
	INCLUDE_DIRECTORIES(${PCL_INCLUDE_DIRS})
    LINK_DIRECTORIES(${PCL_LIBRARY_DIRS})
	ADD_DEFINITIONS(${PCL_DEFINITIONS})
	INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d )
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg/ )


	SET(dbrec3d_pcl_sources
    dbrec3d_scene_to_pc_utils.h       dbrec3d_scene_to_pc_utils.cxx
    dbrec3d_pcl_features_util.h       dbrec3d_pcl_features_util.cxx
    dbrec3d_pcl_codebook_utils.h      dbrec3d_pcl_codebook_utils.cxx
    dbrec3d_pcl_point_types.h
    dbrec3d_pcl_transforms.h
    
    )
    
  AUX_SOURCE_DIRECTORY(Templates dbrec3d_pcl_sources )
  
    
  ADD_LIBRARY(dbrec3d_pcl ${dbrec3d_pcl_sources})
  TARGET_LINK_LIBRARIES(dbrec3d_pcl bvpl_octree dbcll pcl_common pcl_search pcl_features pcl_kdtree pcl_filters)
    
  SUBDIRS(pro)

ENDIF(PCL_FOUND)





