# contrib/edge_det/pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

SET(edge_det_pro_sources
   
   edge_corr_storage.h                     edge_corr_storage.cxx              
   edge_corr_storage_sptr.h
   
   correlate_edge_maps_process.cxx         correlate_edge_maps_process.h
   
)

ADD_LIBRARY(edge_det_pro ${edge_det_pro_sources})

TARGET_LINK_LIBRARIES(edge_det_pro csa bpro1 dbpro)
