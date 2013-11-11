# contrib/edge_det/vis/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

SET(edge_det_vis_sources
   
   edge_corr_displayer.h           edge_corr_displayer.cxx
   
   edge_corr_tableau.h             edge_corr_tableau.cxx
   edge_corr_tableau_sptr.h
   
)

ADD_LIBRARY(edge_det_vis ${edge_det_vis_sources})

TARGET_LINK_LIBRARIES(edge_det_vis )
