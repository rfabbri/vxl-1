

IF(HAS_BGUI3D)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#INCLUDE( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

SET(surfmatch_sources
   surfmatch_main.cxx
   mesh_vis.cxx
   surfmatch.cxx
)

ADD_EXECUTABLE(surfmatch ${surfmatch_sources})

TARGET_LINK_LIBRARIES( surfmatch ITKAlgorithms ITKStatistics ITKIO ITKBasicFilters ITKCommon dbmsh3d dbmsh3d_vis bgui3d vsol )

ENDIF(HAS_BGUI3D)

