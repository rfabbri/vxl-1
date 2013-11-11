# This is /contrib/rfabbri/mw/app/mex
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/rfabbri )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( /home/rfabbri/src/GeometricTools/WildMagic4/LibFoundation )


FIND_PACKAGE(Matlab)
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})


SET( episearch_sources episearch.cxx )
SET( synthetic_data_sources synthetic_data.cxx )
SET( fmatrix_sources fmatrix.cxx )
SET( polybounds_sources polybounds.cxx )
SET( p2conic_dist_sources p2conic_dist.cxx )
SET( fit_conics_sources fit_conics.cxx vxl_mex_utils.cxx vxl_mex_utils.h)
#SET( p2quadric_dist_sources p2quadric_dist.cxx )

ADD_LIBRARY(episearch MODULE ${episearch_sources})
ADD_LIBRARY(synthetic_data MODULE ${synthetic_data_sources})
ADD_LIBRARY(fmatrix MODULE ${fmatrix_sources})
ADD_LIBRARY(polybounds MODULE ${polybounds_sources})
ADD_LIBRARY(p2conic_dist MODULE ${p2conic_dist_sources})
ADD_LIBRARY(fit_conics MODULE ${fit_conics_sources})
#ADD_LIBRARY(p2quadric_dist MODULE ${p2quadric_dist_sources})

TARGET_LINK_LIBRARIES(episearch mw_algo vpgl_algo rrel mvl vgl vsol ${MATLAB_LIBRARIES})

TARGET_LINK_LIBRARIES(synthetic_data mw_algo vpgl_algo vgl vsol ${MATLAB_LIBRARIES})
TARGET_LINK_LIBRARIES(fmatrix vpgl_algo rrel mvl vgl vsol ${MATLAB_LIBRARIES})
TARGET_LINK_LIBRARIES(polybounds mw dbecl ${MATLAB_LIBRARIES})
# /home/rfabbri/src/GeometricTools/WildMagic4/SDK/Library/Release
TARGET_LINK_LIBRARIES(p2conic_dist vgl_algo ${MATLAB_LIBRARIES} )
#TARGET_LINK_LIBRARIES(p2quadric_dist vgl_algo ${MATLAB_LIBRARIES} Wm4Foundation )
TARGET_LINK_LIBRARIES(fit_conics dbgl_algo ${MATLAB_LIBRARIES} )

