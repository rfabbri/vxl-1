# contrib/edge_det/csa/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

SET(csa_sources

   csa_defs.h     csa_types.h
   csa.h          csa.cpp
   Array.h        
   kofn.h         kofn.cpp
   
   match_edge_maps.cpp            match_edge_maps.h
   
)

ADD_LIBRARY(csa ${csa_sources})

TARGET_LINK_LIBRARIES(csa )
