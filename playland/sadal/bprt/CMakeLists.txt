# Brown "playland" plane reconstruction library
# blem/playland/sadal/bprt/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bmvl )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR}/rrel )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR}/mvl )
INCLUDE_DIRECTORIES( ${OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${RPL_INCLUDE_DIR} )




SET(bprt_sources

 bprt_plane_reconst.cxx   bprt_plane_reconst.h
bprt_homog_interface.cxx bprt_homog_interface.h
   
   
)

AUX_SOURCE_DIRECTORY(Templates bprt_sources)

ADD_LIBRARY(bprt  ${bprt_sources})

TARGET_LINK_LIBRARIES(bprt vnl vbl vil vgl vgl_algo bcal mvl rrel)



IF(BUILD_TESTING)
  #SUBDIRS(tests)
ENDIF(BUILD_TESTING)

