# This is config/cmake/Modules/UseVTK.cmake
# Package: vtk 5.0 
# date  : July 7, 2006
# author: Nhon Trinh (ntrinh@lems.brown.edu)


# Find and load the VTK configuration.
FIND_PACKAGE( VTK )
IF (VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
  
  # we're only interested in a subset of VTK's libraries
  SET(VTK_LIBRARIES ${VTK_LIBRARY_DIRS}/libvtksys.a ${VTK_LIBRARY_DIRS}/vtkCommon ${VTK_LIBRARY_DIRS}/vtkFiltering ${VTK_LIBRARY_DIRS}/vtkGraphics ${VTK_LIBRARY_DIRS}/vtkGenericFiltering ${VTK_LIBRARY_DIRS}/vtkImaging )
  
  SET(HAS_VTK "YES")
ENDIF (VTK_FOUND)
