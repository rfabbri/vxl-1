# 3D application
# contrib/knee_cartilage/vis/CMakeLists.txt


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#INCLUDE( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET( dbknee_vis_sources
   dbknee_vis_coord.cxx       dbknee_vis_coord.h
)

ADD_LIBRARY( dbknee_vis  ${dbknee_vis_sources} )

TARGET_LINK_LIBRARIES( dbknee_vis dbknee dbmsh3d_vis bgui3d )

ENDIF (HAS_BGUI3D)
