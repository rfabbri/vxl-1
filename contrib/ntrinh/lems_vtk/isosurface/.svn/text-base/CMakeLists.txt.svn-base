
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#INCLUDE( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

IF(HAS_BGUI3D)

# FIND_PACKAGE( VTK )

# IF (VTK_FOUND)
#  INCLUDE(${VTK_USE_FILE})


SET(isosurface_sources
   isosurface_main.cxx
)

ADD_EXECUTABLE(isosurface ${isosurface_sources})

# TARGET_LINK_LIBRARIES( isosurface vtkGraphics vtkRendering vtkIO dbmsh3d dbmsh3d_vis bgui3d vsol vil3d )
TARGET_LINK_LIBRARIES( isosurface dbmsh3d dbmsh3d_vis dbmsh3d_algovtk bgui3d vsol vil3d )


# ENDIF (VTK_FOUND)

ENDIF(HAS_BGUI3D)
