

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )
SET( dt_sources
    Delaunay.h Delaunay.cpp
    partial_derivatives.h partial_derivatives.cxx
    dbrl_clough_tocher_patch.h dbrl_clough_tocher_patch.cxx

)
#ADD_EXECUTABLE ( dt ${dt_sources})
ADD_LIBRARY(dt  ${dt_sources})
TARGET_LINK_LIBRARIES(dt vul vgl vnl_algo)

#SET( dt_main_sources
#     hullmain.cxx
#)

#ADD_EXECUTABLE ( dt_main ${dt_main_sources})
#TARGET_LINK_LIBRARIES( dt_main dt vul vnl vgl vnl_algo)


#tests
SUBDIRS(tests)
