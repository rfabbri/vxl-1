# contrib/edge_det/vis/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )

set(edge_det_vis_sources
   
   edge_corr_displayer.h           edge_corr_displayer.cxx
   
   edge_corr_tableau.h             edge_corr_tableau.cxx
   edge_corr_tableau_sptr.h
   
)

add_library(edge_det_vis ${edge_det_vis_sources})

target_link_libraries(edge_det_vis bvis1 dbdet dbdet_vis)
