# this is dbrec3d_gui - uses QT


# find the path to the vtk library
include( ${LEMS_CMAKE_DIR}/Modules/UseVTK.cmake )


if(HAS_VTK)
  message("VTK installed")
  include_directories( ${VTK_INCLUDE_DIR} )

  set(dbrec3d_vtk_sources
      dbrec3d_regular_grid_widget.h   dbrec3d_regular_grid_widget.cxx
      dbrec3d_filter_grid_widget.h    dbrec3d_filter_grid_widget.cxx
      dbrec3d_pca_kernels_viewer.h    dbrec3d_pca_kernels_viewer.cxx
      dbrec3d_vtk_boxm_utils.h        dbrec3d_vtk_boxm_utils.cxx
      dbrec3d_octree_widget.h         dbrec3d_octree_widget.cxx
      dbrec3d_kernel_viewer.h         dbrec3d_kernel_viewer.cxx
      dbrec3d_region_viewer.h         dbrec3d_region_viewer.cxx
      )
      
  qt_wrap_cpp(dbrec3d_vtk dbrec3d_qvtk_moc_files
          dbrec3d_regular_grid_widget.h
          dbrec3d_filter_grid_widget.h
          dbrec3d_pca_kernels_viewer.h
          dbrec3d_kernel_viewer.h
          dbrec3d_octree_widget.h
          dbrec3d_region_viewer.h
      )
      
  set(dbrec3d_vtk_sources ${dbrec3d_vtk_sources} ${dbrec3d_qvtk_moc_files} )    
  
  add_library(dbrec3d_vtk  ${dbrec3d_vtk_sources})
  target_link_libraries(dbrec3d_vtk dbrec3d_gui vtkCommon vtkFiltering vtkImaging vtkGraphics vtkGenericFiltering vtkRendering vtkVolumeRendering QVTK)  
  
endif(HAS_VTK)



