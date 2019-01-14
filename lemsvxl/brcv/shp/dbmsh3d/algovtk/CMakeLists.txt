# This is dbmsh3d/algovtk/CMakeLists.txt
# Date: July 8, 2006


# VTK-based Brown Mesh Algorithm Library
# This library requires the VTK (Visualization Toolkit) library.
# Download it from http://public.kitware.com/VTK/
# Known working version: 5.0


# Optionally Build this library
option(BUILD_DBMSH3D_ALGOVTK "Build VTK-based Brown Mesh Algorithm Library" NO)

if(BUILD_DBMSH3D_ALGOVTK)

# find the path to the vtk library
include( ${LEMS_CMAKE_DIR}/Modules/UseVTK.cmake )

  
if(HAS_VTK)

# include_directories( ${LEMS_SOURCE_DIR}/basic )

include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )

set(dbmsh3d_algovtk_sources
  dbmsh3d_isosurface.cxx          dbmsh3d_isosurface.h
)
aux_source_directory(Templates dbmsh3d_algovtk_sources)

add_library(dbmsh3d_algovtk  ${dbmsh3d_algovtk_sources})

target_link_libraries(dbmsh3d_algovtk dbmsh3d vil3d ${VTK_LIBRARIES} )

# if( BUILD_EXAMPLES )
#  add_subdirectory( examples )
# endif( BUILD_EXAMPLES )

add_subdirectory( pro )


endif(HAS_VTK)

endif(BUILD_DBMSH3D_ALGOVTK)
