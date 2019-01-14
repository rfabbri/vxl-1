# these are bvpl executables

include(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
include(${CMAKE_ROOT}/Modules/FindGLUT.cmake)
include(${CMAKE_ROOT}/Modules/FindQt.cmake)
include(${LEMS_CMAKE_DIR}/Modules/UseVTK.cmake)
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg/ )
include_directories( ${LEMS_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d )

find_package( PCL 1.0 )

if(PCL_FOUND)
	message("PCL library found")
	include_directories(${PCL_INCLUDE_DIRS})
    link_directories(${PCL_LIBRARY_DIRS})
	add_definitions(${PCL_DEFINITIONS})

endif(PCL_FOUND)
    


if(QT4_INSTALLED)
  message("qt4 installed")
  
  include_directories( ${QWT_INCLUDE_DIR} )
  include_directories( ${QGLVIEWER_INCLUDE_DIR} )
  include_directories( ${QGLVIEWER_INCLUDE_DIR}/QGLViewer )
  include_directories( ${QT_INCLUDE_DIR} ${QT_QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} 
                           ${QT_QTCORE_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

 
  add_executable( dbrec3d_render_vector_field dbrec3d_render_vector_field.cxx)
  target_link_libraries( dbrec3d_render_vector_field bvpl bvpl_octree dbrec3d_gui ) 
  
  add_executable(dbrec3d_render_points dbrec3d_render_points.cxx)
  target_link_libraries(dbrec3d_render_points boxm dbrec3d_gui)
  
  add_executable( dbrec3d_render_id_scene_as_points dbrec3d_render_id_scene_as_points.cxx)
  target_link_libraries(dbrec3d_render_id_scene_as_points bvpl_octree dbrec3d_gui )
  
  add_executable( dbrec3d_scene_statistics dbrec3d_scene_statistics.cxx)
  target_link_libraries( dbrec3d_scene_statistics bvpl bvpl_octree dbrec3d_gui ) 

  add_executable( dbrec3d_main dbrec3d_main.cxx)
  target_link_libraries( dbrec3d_main bvpl bvpl_octree dbrec3d_gui) 
  
  add_executable( dbrec3d_plot dbrec3d_plot.cxx)
  target_link_libraries( dbrec3d_plot dbrec3d_gui) 
  
  add_executable( dbrec3d_pc_statistics dbrec3d_pc_statistics.cxx)
  target_link_libraries( dbrec3d_pc_statistics dbrec3d_gui) 
  
  
  if(PCL_FOUND)
    add_executable(dbrec3d_display_gradients_as_pc dbrec3d_display_gradients_as_pc.cxx)
    target_link_libraries(dbrec3d_display_gradients_as_pc dbrec3d_pcl bof ${PCL_LIBRARIES} ${PCL_VISUALIZATION_LIBRARY})
    
    add_executable( dbrec3d_scene_objects_to_pcd  dbrec3d_scene_objects_to_pcd.cxx)
    target_link_libraries( dbrec3d_scene_objects_to_pcd dbrec3d_pcl bof ${PCL_IO_LIBRARY} ${PCL_SEARCH_LIBRARY} ${PCL_KDTREE_LIBRARY} ${PCL_FEATURES_LIBRARY})
    
    message( ${PCL_IO_LIBRARY} ${PCL_SEARCH_LIBRARY} ${PCL_KDTREE_LIBRARY} ${PCL_FEATURES_LIBRARY} )
    
  endif(PCL_FOUND)

  
  if(HAS_VTK)
    add_executable( dbrec3d_render_vtk_regular_grid dbrec3d_render_vtk_regular_grid.cxx)
    target_link_libraries( dbrec3d_render_vtk_regular_grid dbrec3d_vtk)
    
    #add_executable( dbrec3d_vtk_render_octree dbrec3d_vtk_render_octree.cxx)
    #target_link_libraries( dbrec3d_vtk_render_octree dbrec3d_vtk vtkCharts)
    
    add_executable(dbrec3d_render_pca_patches dbrec3d_render_pca_patches.cxx)
    target_link_libraries( dbrec3d_render_pca_patches dbrec3d_vtk)
    
    add_executable(dbrec3d_render_kernels dbrec3d_render_kernels.cxx)
    target_link_libraries( dbrec3d_render_kernels dbrec3d_vtk)
    
    add_executable(dbrec3d_render_pca_k_means dbrec3d_render_pca_k_means.cxx)
    target_link_libraries(dbrec3d_render_pca_k_means dbrec3d_vtk)
    
    add_executable(dbrec3d_render_taylor_k_means dbrec3d_render_taylor_k_means.cxx)
    target_link_libraries(dbrec3d_render_taylor_k_means dbrec3d_vtk)
    
    add_executable(dbrec3d_render_L1_taylor dbrec3d_render_L1_taylor.cxx)
    target_link_libraries(dbrec3d_render_L1_taylor dbrec3d_vtk)
    
    add_executable(dbrec3d_display_objects_from_bbox dbrec3d_display_objects_from_bbox.cxx)
    target_link_libraries(dbrec3d_display_objects_from_bbox dbrec3d_vtk bof)
    
    
  endif(HAS_VTK)
  
  
endif(QT4_INSTALLED)


