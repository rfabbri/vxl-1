# This is /contrib/rfabbri/mw/app/mex
include_directories( ${LEMS_SOURCE_DIR}/contrib/rfabbri )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/bmvgd)
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( /home/rfabbri/src/GeometricTools/WildMagic4/LibFoundation )


find_package(Matlab)
include_directories(${Matlab_INCLUDE_DIRS})


set( episearch_sources episearch.cxx )
set( synthetic_data_sources synthetic_data.cxx )
set( fmatrix_sources fmatrix.cxx )
set( polybounds_sources polybounds.cxx )
set( p2conic_dist_sources p2conic_dist.cxx )
set( fit_conics_sources fit_conics.cxx vxl_mex_utils.cxx vxl_mex_utils.h)
#set( p2quadric_dist_sources p2quadric_dist.cxx )

add_library(episearch MODULE ${episearch_sources})
add_library(synthetic_data MODULE ${synthetic_data_sources})
add_library(fmatrix MODULE ${fmatrix_sources})
add_library(polybounds MODULE ${polybounds_sources})
add_library(p2conic_dist MODULE ${p2conic_dist_sources})
add_library(fit_conics MODULE ${fit_conics_sources})
#add_library(p2quadric_dist MODULE ${p2quadric_dist_sources})

target_link_libraries(episearch mw_algo vpgl_algo rrel mvl vgl vsol ${MATLAB_LIBRARIES})

target_link_libraries(synthetic_data mw_algo vpgl_algo vgl vsol ${MATLAB_LIBRARIES})
target_link_libraries(fmatrix vpgl_algo rrel mvl vgl vsol ${MATLAB_LIBRARIES})
target_link_libraries(polybounds mw becld ${MATLAB_LIBRARIES})
# /home/rfabbri/src/GeometricTools/WildMagic4/SDK/Library/Release
target_link_libraries(p2conic_dist vgl_algo ${MATLAB_LIBRARIES} )
#target_link_libraries(p2quadric_dist vgl_algo ${MATLAB_LIBRARIES} Wm4Foundation )
target_link_libraries(fit_conics bgld_algo ${MATLAB_LIBRARIES} )

