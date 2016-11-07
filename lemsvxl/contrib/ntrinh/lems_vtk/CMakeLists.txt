# lems code linked with vtk

project(lems_itk)

# Find and load the VTK configuration.
# find_package(VTK)
  
  #if(NOT VTK_DIR)
  #  message(FATAL_ERROR "Please set VTK_DIR.")
  #endif(NOT VTK_DIR)

# if (VTK_FOUND)  
  
#  include(${VTK_USE_FILE})
#  add_subdirectory( isosurface )
  
# endif (VTK_FOUND)

# if ( HAS_VTK )
  add_subdirectory( isosurface )
# endif (HAS_VTK )
