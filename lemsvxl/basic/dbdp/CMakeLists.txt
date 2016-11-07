# lemsvxlsrc/basic/dbdp/CMakeLists.txt
# The Development Brown Dynamic Programming

set( dbdp_sources
dbdp_engine.h   dbdp_engine.cxx
dbdp_cost.h
)

add_library(dbdp  ${dbdp_sources})
  
target_link_libraries( dbdp vcl)


