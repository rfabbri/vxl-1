# This is brcv/shp/dbskfg/pro/CMakeLists.txt
# Brown 2D Composite_graph library

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(con_graph_pro_sources
  
   dbskfg_form_containment_graph_process.h 
   dbskfg_form_containment_graph_process.cxx
  
  dbskfg_form_containment_graph_region_process.h 
  dbskfg_form_containment_graph_region_process.cxx

 # dbskfg_form_containment_graph_test_process.h 
 # dbskfg_form_containment_graph_test_process.cxx

)

ADD_LIBRARY(con_graph_pro ${con_graph_pro_sources})

TARGET_LINK_LIBRARIES(con_graph_pro con_graph )
