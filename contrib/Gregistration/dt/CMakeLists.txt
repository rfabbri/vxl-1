

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/basic/ )
include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )
set( dt_sources
    Delaunay.h Delaunay.cpp
    partial_derivatives.h partial_derivatives.cxx
    dbrl_clough_tocher_patch.h dbrl_clough_tocher_patch.cxx

)
#add_executable ( dt ${dt_sources})
add_library(dt  ${dt_sources})
target_link_libraries(dt vul vgl vnl_algo)

#set( dt_main_sources
#     hullmain.cxx
#)

#add_executable ( dt_main ${dt_main_sources})
#target_link_libraries( dt_main dt vul vnl vgl vnl_algo)


#tests
add_subdirectory(tests)
