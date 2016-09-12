# This is /db3p/rply/CMakeLists.txt
# library for reading and writing .PLY file

set(rply_sources
  rply.h
  rply.c
  )
 
add_library(rply ${rply_sources})
