

if(HAS_BGUI3D)

include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${LEMS_SOURCE_DIR}/contrib/ntrinh )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#include( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

set(surfmatch_sources
   surfmatch_main.cxx
   mesh_vis.cxx
   surfmatch.cxx
)

add_executable(surfmatch ${surfmatch_sources})

target_link_libraries( surfmatch ITKAlgorithms ITKStatistics ITKIO ITKBasicFilters ITKCommon dbmsh3d dbmsh3d_vis bgui3d vsol )

endif(HAS_BGUI3D)

