# This is /db3p/mesh_triangulate/CMakeLists.txt
# Library for polygonal mesh triangularization

set ( mesh_tri_sources
  mesh_tri.cxx    mesh_tri.h
  )
 
add_library(mesh_tri ${mesh_tri_sources})
