# contrib/maruthi/CMakeLists.txt
# This file is used to build the en250 project

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/contrib/maruthi )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/basic )

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )

add_executable(xdetect detect_xgraph_cartilage.cxx detect_xgraph_app.cxx dbsks_xshock_app_likelihood.cxx dbsks_xshock_detector.cxx )
target_link_libraries(xdetect vox_detect_xgraph_lib dborl_algo dbsks dbsks_pro dbsksp_xio vidpro1)

