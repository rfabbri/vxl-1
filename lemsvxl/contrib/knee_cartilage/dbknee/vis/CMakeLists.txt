# 3D application
# contrib/knee_cartilage/vis/CMakeLists.txt


include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#include( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

if (HAS_BGUI3D)

set( dbknee_vis_sources
   dbknee_vis_coord.cxx       dbknee_vis_coord.h
)

add_library( dbknee_vis  ${dbknee_vis_sources} )

target_link_libraries( dbknee_vis dbknee dbmsh3d_vis bgui3d )

endif (HAS_BGUI3D)
