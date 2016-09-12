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




set(bprt_sources

 bprt_plane_reconst.cxx   bprt_plane_reconst.h
bprt_homog_interface.cxx bprt_homog_interface.h
   
   
)

aux_source_directory(Templates bprt_sources)

add_library(bprt  ${bprt_sources})

target_link_libraries(bprt vnl vbl vil vgl vgl_algo bcal mvl rrel)



if(BUILD_TESTING)
  #add_subdirectory(tests)
endif(BUILD_TESTING)

