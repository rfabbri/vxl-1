# this is dbrec3d_gui - uses QT


# find the path to the vtk library
INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseVTK.cmake )


IF(HAS_VTK)
  MESSAGE("VTK installed")
  INCLUDE_DIRECTORIES( ${VTK_INCLUDE_DIR} )

  SET(dbrec3d_vtk_sources
      dbrec3d_regular_grid_widget.h   dbrec3d_regular_grid_widget.cxx
      dbrec3d_filter_grid_widget.h    dbrec3d_filter_grid_widget.cxx
      dbrec3d_pca_kernels_viewer.h    dbrec3d_pca_kernels_viewer.cxx
      dbrec3d_vtk_boxm_utils.h        dbrec3d_vtk_boxm_utils.cxx
      dbrec3d_octree_widget.h         dbrec3d_octree_widget.cxx
      dbrec3d_kernel_viewer.h         dbrec3d_kernel_viewer.cxx
      dbrec3d_region_viewer.h         dbrec3d_region_viewer.cxx
      )
      
  QT_WRAP_CPP(dbrec3d_vtk dbrec3d_qvtk_moc_files
          dbrec3d_regular_grid_widget.h
          dbrec3d_filter_grid_widget.h
          dbrec3d_pca_kernels_viewer.h
          dbrec3d_kernel_viewer.h
          dbrec3d_octree_widget.h
          dbrec3d_region_viewer.h
      )
      
  SET(dbrec3d_vtk_sources ${dbrec3d_vtk_sources} ${dbrec3d_qvtk_moc_files} )    
  
  ADD_LIBRARY(dbrec3d_vtk  ${dbrec3d_vtk_sources})
  TARGET_LINK_LIBRARIES(dbrec3d_vtk dbrec3d_gui vtkCommon vtkFiltering vtkImaging vtkGraphics vtkGenericFiltering vtkRendering vtkVolumeRendering QVTK)  
  
ENDIF(HAS_VTK)



