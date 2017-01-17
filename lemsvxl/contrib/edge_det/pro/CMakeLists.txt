# contrib/edge_det/pro/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )



set(edge_det_pro_sources
   
   edge_corr_storage.h                     edge_corr_storage.cxx              
   edge_corr_storage_sptr.h
   
   correlate_edge_maps_process.cxx         correlate_edge_maps_process.h
   
)

add_library(edge_det_pro ${edge_det_pro_sources})

target_link_libraries(edge_det_pro csa bpro1 bprod dbdet)
