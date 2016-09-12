# lems code linked with vtk

project(lems_itk)

# Find and load the VTK configuration.
# find_package(VTK)
  
  #if(NOT VTK_DIR)
  #  message(FATAL_ERROR "Please set VTK_DIR.")
  #endif(NOT VTK_DIR)

# if (VTK_FOUND)  
  
#  include(${VTK_USE_FILE})
#  subdirs ( isosurface )
  
# endif (VTK_FOUND)

# if ( HAS_VTK )
  subdirs ( isosurface )
# endif (HAS_VTK )
