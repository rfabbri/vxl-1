# this is dbrec3d_pcl uses the point cloud library

find_package( PCL 1.0 )

if(PCL_FOUND)
	
  message("PCL library found")
  add_definitions(${PCL_DEFINITIONS})
	include_directories(${PCL_INCLUDE_DIRS})
    link_directories(${PCL_LIBRARY_DIRS})
	add_definitions(${PCL_DEFINITIONS})
	include_directories( ${LEMS_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d )
  include_directories( ${LEMS_SOURCE_DIR}/brcv/seg/ )


	set(dbrec3d_pcl_sources
    dbrec3d_scene_to_pc_utils.h       dbrec3d_scene_to_pc_utils.cxx
    dbrec3d_pcl_features_util.h       dbrec3d_pcl_features_util.cxx
    dbrec3d_pcl_codebook_utils.h      dbrec3d_pcl_codebook_utils.cxx
    dbrec3d_pcl_point_types.h
    dbrec3d_pcl_transforms.h
    
    )
    
  aux_source_directory(Templates dbrec3d_pcl_sources )
  
    
  add_library(dbrec3d_pcl ${dbrec3d_pcl_sources})
  target_link_libraries(dbrec3d_pcl bvpl_octree dbcll pcl_common pcl_search pcl_features pcl_kdtree pcl_filters)
    
  add_subdirectory(pro)

endif(PCL_FOUND)





