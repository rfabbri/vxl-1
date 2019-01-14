# This is brcv/shp/dbskfg/pro/CMakeLists.txt
# Brown 2D Composite_graph library

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )

set(con_graph_pro_sources
  
   dbskfg_form_containment_graph_process.h 
   dbskfg_form_containment_graph_process.cxx
  
  dbskfg_form_containment_graph_region_process.h 
  dbskfg_form_containment_graph_region_process.cxx

 # dbskfg_form_containment_graph_test_process.h 
 # dbskfg_form_containment_graph_test_process.cxx

)

add_library(con_graph_pro ${con_graph_pro_sources})

target_link_libraries(con_graph_pro con_graph )
