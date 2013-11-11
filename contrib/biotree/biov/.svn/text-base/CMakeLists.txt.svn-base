# library for storing / loading 3d image 
OPTION ( BUILD_CONTRIB_BIOTREE_biov "Build contrib/biotree/biov" ON)
IF ( BUILD_CONTRIB_BIOTREE_biov )

INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic)

 SET(biov_sources
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

 ADD_LIBRARY(biov  ${biov_sources})

 TARGET_LINK_LIBRARIES(biov bgui3d vgui vnl)

  IF ( BUILD_EXAMPLES )
   SUBDIRS ( examples )
  ENDIF ( BUILD_EXAMPLES )
  SUBDIRS(io)

ENDIF ( BUILD_CONTRIB_BIOTREE_biov )
