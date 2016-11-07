# dbmsh3d - Brown 3D Mesh Library
# brcv/shp/dbmsh3d/CMakeLists.txt

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/brcv/shp)

set(dbmsh3d_sources

                                     dbmsh3d_utils.h
                                     dbmsh3d_ptr_list.h

   dbmsh3d_vertex.cxx                dbmsh3d_vertex.h
   dbmsh3d_richvertex.cxx            dbmsh3d_richvertex.h
   
   dbmsh3d_halfedge.cxx              dbmsh3d_halfedge.h
   dbmsh3d_edge.cxx                  dbmsh3d_edge.h
   dbmsh3d_face.cxx                  dbmsh3d_face.h
   dbmsh3d_face_mc.cxx               dbmsh3d_face_mc.h
   dbmsh3d_textured_face_mc.cxx      dbmsh3d_textured_face_mc.h
   dbmsh3d_richface.cxx              dbmsh3d_richface.h

   dbmsh3d_pt_set.cxx                dbmsh3d_pt_set.h
   dbmsh3d_mesh.cxx                  dbmsh3d_mesh.h
   dbmsh3d_mesh_mc.cxx               dbmsh3d_mesh_mc.h
   dbmsh3d_textured_mesh_mc.cxx      dbmsh3d_textured_mesh_mc.h

   dbmsh3d_richmesh.cxx              dbmsh3d_richmesh.h

   dbmsh3d_he_mesh.cxx               dbmsh3d_he_mesh.h
   dbmsh3d_m2t_mesh.cxx              dbmsh3d_m2t_mesh.h

   dbmsh3d_mesh_conn_recov.cxx

   dbmsh3d_node.cxx                  dbmsh3d_node.h
   dbmsh3d_curve.cxx                 dbmsh3d_curve.h
   dbmsh3d_sheet.cxx                 dbmsh3d_sheet.h
   dbmsh3d_graph.cxx                 dbmsh3d_graph.h
   dbmsh3d_hypg.cxx                  dbmsh3d_hypg.h

   dbmsh3d_sg3pi.cxx                 dbmsh3d_sg3pi.h
)

add_library( dbmsh3d  ${dbmsh3d_sources})

target_link_libraries( dbmsh3d vsol dbgl dbul rply vil)

add_subdirectory( algo )
add_subdirectory( pro )
add_subdirectory( vis )
add_subdirectory( cmdproc )
if (BUILD_TESTING)
    add_subdirectory( tests )
endif (BUILD_TESTING)
if (BUILD_EXAMPLES)
    add_subdirectory( examples )
endif (BUILD_EXAMPLES)



