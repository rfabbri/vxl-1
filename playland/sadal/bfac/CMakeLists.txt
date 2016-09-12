# Brown "playland" plane reconstruction library
# blem/playland/sadal/bprt/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bmvl )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR}/rrel )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR}/mvl )
include_directories( ${OXL_INCLUDE_DIR} )
include_directories( ${RPL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bmvl )




set(bfac_sources

bfac_factorization.cxx   bfac_factorization.h
   
   
)

aux_source_directory(Templates bfac_sources)

add_library(bfac  ${bfac_sources})

target_link_libraries(bfac vnl vbl vgl vgl_algo brct  bcal mvl rrel mbl )



