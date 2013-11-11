# 3D application
# contrib/geodesic_mesh/CMakeLists.txt
#
# Added by Ming-Ching Chang, Aug 16, 2005
#
# This is an implementation of Chen & Han's algorithm for computing the shortest paths on a 
# polyhedral surface from one point to all vertices. Based on a senior honor's thesis by Biliana 
# Kaneva, Smith College, April 2000.  Supervised by Joseph O'Rourke. See our paper, "An 
# Implementation of Chen & Han's Shortest Paths Algorithm." CCCG August 2000.  Available via 
# http://cs.smith.edu/~orourke/.
#
#

SET ( geodesic_mesh_sources

   sp.cxx
   cone.cxx              cone.h
   ctree.cxx             ctree.h
   llist.cxx             llist.h
                         macros.h
   mathops.cxx           mathops.h
   structs.cxx           structs.h
)


ADD_EXECUTABLE ( geodesic_mesh ${geodesic_mesh_sources})

TARGET_LINK_LIBRARIES( geodesic_mesh vcl )

