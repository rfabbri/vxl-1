# library for storing / loading 3d image 
option ( BUILD_CONTRIB_BIOTREE_biov "Build contrib/biotree/biov" ON)
if ( BUILD_CONTRIB_BIOTREE_biov )

include_directories(${lems_SOURCE_DIR}/basic)

 set(biov_sources
  biov_examiner_tableau.cxx     biov_examiner_tableau.h     biov_examiner_tableau_sptr.h
  biov_transfer_function.h      biov_transfer_function.cxx
  CvrGIMPGradient.h             GIMPGradient.cpp
  biov_colormaps.h
  BLUE_RED.h
  PHYSICS.h
  GLOW.h
  GREY.h
  SEISMIC.h
  STANDARD.h
  TEMPERATURE.h
 )

 add_library(biov  ${biov_sources})

 target_link_libraries(biov bgui3d vgui vnl)

  if ( BUILD_EXAMPLES )
   add_subdirectory( examples )
  endif ( BUILD_EXAMPLES )
  add_subdirectory(io)

endif ( BUILD_CONTRIB_BIOTREE_biov )
