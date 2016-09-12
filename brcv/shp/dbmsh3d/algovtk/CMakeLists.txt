# This is dbmsh3d/algovtk/CMakeLists.txt
# Date: July 8, 2006


# VTK-based Brown Mesh Algorithm Library
# This library requires the VTK (Visualization Toolkit) library.
# Download it from http://public.kitware.com/VTK/
# Known working version: 5.0


# Optionally Build this library
OPTION(BUILD_DBMSH3D_ALGOVTK "Build VTK-based Brown Mesh Algorithm Library" NO)

IF(BUILD_DBMSH3D_ALGOVTK)

# find the path to the vtk library
INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseVTK.cmake )

  
IF(HAS_VTK)

# INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

SET(dbmsh3d_algovtk_sources
  dbmsh3d_isosurface.cxx          dbmsh3d_isosurface.h
)
AUX_SOURCE_DIRECTORY(Templates dbmsh3d_algovtk_sources)

ADD_LIBRARY(dbmsh3d_algovtk  ${dbmsh3d_algovtk_sources})

TARGET_LINK_LIBRARIES(dbmsh3d_algovtk dbmsh3d vil3d ${VTK_LIBRARIES} )

# IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
# ENDIF( BUILD_EXAMPLES )

SUBDIRS( pro )


ENDIF(HAS_VTK)

ENDIF(BUILD_DBMSH3D_ALGOVTK)
