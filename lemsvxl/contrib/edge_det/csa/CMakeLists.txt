# contrib/edge_det/csa/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )

set(csa_sources

   csa_defs.h     csa_types.h
   csa.h          csa.cpp
   Array.h        
   kofn.h         kofn.cpp
   
   match_edge_maps.cpp            match_edge_maps.h
   
)

add_library(csa ${csa_sources})

target_link_libraries(csa )
