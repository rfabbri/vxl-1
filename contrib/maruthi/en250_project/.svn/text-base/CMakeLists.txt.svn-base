# contrib/maruthi/CMakeLists.txt
# This file is used to build the en250 project

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

ADD_EXECUTABLE(xdetect detect_xgraph_cartilage.cxx detect_xgraph_app.cxx dbsks_xshock_app_likelihood.cxx dbsks_xshock_detector.cxx )
TARGET_LINK_LIBRARIES(xdetect vox_detect_xgraph_lib dborl_algo dbsks dbsks_pro dbsksp_xio vidpro1)

