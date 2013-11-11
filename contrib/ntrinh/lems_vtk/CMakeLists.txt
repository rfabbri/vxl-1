# lems code linked with vtk

PROJECT(lems_itk)

# Find and load the VTK configuration.
# FIND_PACKAGE(VTK)
  
  #IF(NOT VTK_DIR)
  #  MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
  #ENDIF(NOT VTK_DIR)

# IF (VTK_FOUND)  
  
#  INCLUDE(${VTK_USE_FILE})
#  SUBDIRS ( isosurface )
  
# ENDIF (VTK_FOUND)

# IF ( HAS_VTK )
  SUBDIRS ( isosurface )
# ENDIF (HAS_VTK )
