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
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bmvl )




SET(bfac_sources

bfac_factorization.cxx   bfac_factorization.h
   
   
)

AUX_SOURCE_DIRECTORY(Templates bfac_sources)

ADD_LIBRARY(bfac  ${bfac_sources})

TARGET_LINK_LIBRARIES(bfac vnl vbl vgl vgl_algo brct  bcal mvl rrel mbl )



