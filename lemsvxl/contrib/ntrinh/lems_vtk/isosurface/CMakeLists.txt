
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_MUL_INCLUDE_DIR} )


include_directories( ${LEMS_SOURCE_DIR}/contrib/ntrinh )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#include( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

if(HAS_BGUI3D)

# find_package( VTK )

# if (VTK_FOUND)
#  include(${VTK_USE_FILE})


set(isosurface_sources
   isosurface_main.cxx
)

add_executable(isosurface ${isosurface_sources})

# target_link_libraries( isosurface vtkGraphics vtkRendering vtkIO dbmsh3d dbmsh3d_vis bgui3d vsol vil3d )
target_link_libraries( isosurface dbmsh3d dbmsh3d_vis dbmsh3d_algovtk bgui3d vsol vil3d )


# endif (VTK_FOUND)

endif(HAS_BGUI3D)
