# these are bvpl executables

INCLUDE(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/FindGLUT.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/FindQt.cmake)
INCLUDE(${LEMS_CMAKE_DIR}/Modules/UseVTK.cmake)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d )

FIND_PACKAGE( PCL 1.0 )

IF(PCL_FOUND)
	MESSAGE("PCL library found")
	INCLUDE_DIRECTORIES(${PCL_INCLUDE_DIRS})
    LINK_DIRECTORIES(${PCL_LIBRARY_DIRS})
	ADD_DEFINITIONS(${PCL_DEFINITIONS})

ENDIF(PCL_FOUND)
    


IF(QT4_INSTALLED)
  MESSAGE("qt4 installed")
  
  INCLUDE_DIRECTORIES( ${QWT_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${QGLVIEWER_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${QGLVIEWER_INCLUDE_DIR}/QGLViewer )
  INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} 
                           ${QT_QTCORE_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

 
  ADD_EXECUTABLE( dbrec3d_render_vector_field dbrec3d_render_vector_field.cxx)
  TARGET_LINK_LIBRARIES( dbrec3d_render_vector_field bvpl bvpl_octree dbrec3d_gui ) 
  
  ADD_EXECUTABLE(dbrec3d_render_points dbrec3d_render_points.cxx)
  TARGET_LINK_LIBRARIES(dbrec3d_render_points boxm dbrec3d_gui)
  
  ADD_EXECUTABLE( dbrec3d_render_id_scene_as_points dbrec3d_render_id_scene_as_points.cxx)
  TARGET_LINK_LIBRARIES(dbrec3d_render_id_scene_as_points bvpl_octree dbrec3d_gui )
  
  ADD_EXECUTABLE( dbrec3d_scene_statistics dbrec3d_scene_statistics.cxx)
  TARGET_LINK_LIBRARIES( dbrec3d_scene_statistics bvpl bvpl_octree dbrec3d_gui ) 

  ADD_EXECUTABLE( dbrec3d_main dbrec3d_main.cxx)
  TARGET_LINK_LIBRARIES( dbrec3d_main bvpl bvpl_octree dbrec3d_gui) 
  
  ADD_EXECUTABLE( dbrec3d_plot dbrec3d_plot.cxx)
  TARGET_LINK_LIBRARIES( dbrec3d_plot dbrec3d_gui) 
  
  ADD_EXECUTABLE( dbrec3d_pc_statistics dbrec3d_pc_statistics.cxx)
  TARGET_LINK_LIBRARIES( dbrec3d_pc_statistics dbrec3d_gui) 
  
  
  IF(PCL_FOUND)
    ADD_EXECUTABLE(dbrec3d_display_gradients_as_pc dbrec3d_display_gradients_as_pc.cxx)
    TARGET_LINK_LIBRARIES(dbrec3d_display_gradients_as_pc dbrec3d_pcl bof ${PCL_LIBRARIES} ${PCL_VISUALIZATION_LIBRARY})
    
    ADD_EXECUTABLE( dbrec3d_scene_objects_to_pcd  dbrec3d_scene_objects_to_pcd.cxx)
    TARGET_LINK_LIBRARIES( dbrec3d_scene_objects_to_pcd dbrec3d_pcl bof ${PCL_IO_LIBRARY} ${PCL_SEARCH_LIBRARY} ${PCL_KDTREE_LIBRARY} ${PCL_FEATURES_LIBRARY})
    
    MESSAGE( ${PCL_IO_LIBRARY} ${PCL_SEARCH_LIBRARY} ${PCL_KDTREE_LIBRARY} ${PCL_FEATURES_LIBRARY} )
    
  ENDIF(PCL_FOUND)

  
  IF(HAS_VTK)
    ADD_EXECUTABLE( dbrec3d_render_vtk_regular_grid dbrec3d_render_vtk_regular_grid.cxx)
    TARGET_LINK_LIBRARIES( dbrec3d_render_vtk_regular_grid dbrec3d_vtk)
    
    #ADD_EXECUTABLE( dbrec3d_vtk_render_octree dbrec3d_vtk_render_octree.cxx)
    #TARGET_LINK_LIBRARIES( dbrec3d_vtk_render_octree dbrec3d_vtk vtkCharts)
    
    ADD_EXECUTABLE(dbrec3d_render_pca_patches dbrec3d_render_pca_patches.cxx)
    TARGET_LINK_LIBRARIES( dbrec3d_render_pca_patches dbrec3d_vtk)
    
    ADD_EXECUTABLE(dbrec3d_render_kernels dbrec3d_render_kernels.cxx)
    TARGET_LINK_LIBRARIES( dbrec3d_render_kernels dbrec3d_vtk)
    
    ADD_EXECUTABLE(dbrec3d_render_pca_k_means dbrec3d_render_pca_k_means.cxx)
    TARGET_LINK_LIBRARIES(dbrec3d_render_pca_k_means dbrec3d_vtk)
    
    ADD_EXECUTABLE(dbrec3d_render_taylor_k_means dbrec3d_render_taylor_k_means.cxx)
    TARGET_LINK_LIBRARIES(dbrec3d_render_taylor_k_means dbrec3d_vtk)
    
    ADD_EXECUTABLE(dbrec3d_render_L1_taylor dbrec3d_render_L1_taylor.cxx)
    TARGET_LINK_LIBRARIES(dbrec3d_render_L1_taylor dbrec3d_vtk)
    
    ADD_EXECUTABLE(dbrec3d_display_objects_from_bbox dbrec3d_display_objects_from_bbox.cxx)
    TARGET_LINK_LIBRARIES(dbrec3d_display_objects_from_bbox dbrec3d_vtk bof)
    
    
  ENDIF(HAS_VTK)
  
  
ENDIF(QT4_INSTALLED)


