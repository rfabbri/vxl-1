# Brown edge tracking using particle library 
# brcv/trk/dbetrk/CMakeLists.txt
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )

SET(dbetrk_sources
  
   dbetrk_edge.cxx dbetrk_edge.h dbetrk_edge_sptr.h
   dbetrk_track.cxx dbetrk_track.h 
)
AUX_SOURCE_DIRECTORY(Templates dbetrk_sources)

ADD_LIBRARY(dbetrk  ${dbetrk_sources})

TARGET_LINK_LIBRARIES(dbetrk vbl vdgl vtol vsol vil vnl  btol mvl mbl vsl brip)

SUBDIRS(pro)
IF (VXL_VGUI_FOUND)
SUBDIRS(vis)
ENDIF (VXL_VGUI_FOUND)

IF(BUILD_TESTING)
        SUBDIRS(tests)
ENDIF(BUILD_TESTING)
