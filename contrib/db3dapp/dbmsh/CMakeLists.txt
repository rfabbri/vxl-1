# 3D mesh library
# brcv/shp/dbmsh/CMakeLists.txt


SET(dbmsh_sources

   dbmsh_item.h            dbmsh_item.cxx
   dbmsh_node.h            dbmsh_node.cxx
   dbmsh_link.h            dbmsh_link.cxx
   dbmsh_face.h            dbmsh_face.cxx

)

ADD_LIBRARY(dbmsh  ${dbmsh_sources})

SUBDIRS( vis )


