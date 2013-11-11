# Shock library
# brcv/shp/dbsk2d/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

PROJECT ( dbsk2d )

SET( dbsk2d_sources

# Basic defines and utility functions
dbsk2d_assert.h                   dbsk2d_assert.cxx
dbsk2d_exception.h                dbsk2d_exception.cxx

dbsk2d_defines.h
dbsk2d_fuzzy_boolean.h
dbsk2d_geometry_utils.h           dbsk2d_geometry_utils.cxx
dbsk2d_utils.h                    dbsk2d_utils.cxx
dbsk2d_file_io.h                  dbsk2d_file_io.cxx

dbsk2d_base_gui_geometry.h

dbsk2d_distance.h                 dbsk2d_distance.cxx
dbsk2d_closest_point.h            dbsk2d_closest_point.cxx

# ***********************
# Shock graph classes
# ***********************

# Shock graph data structures                               
dbsk2d_shock_node.h    dbsk2d_shock_node.cxx
dbsk2d_shock_node_sptr.h               
dbsk2d_shock_edge.h    dbsk2d_shock_edge.cxx               
dbsk2d_shock_edge_sptr.h 
dbsk2d_shock_graph.h   dbsk2d_shock_graph.cxx
dbsk2d_shock_graph_sptr.h  

# Coarse intrinsic shock graph classes
dbsk2d_shock_ishock_node.h         dbsk2d_shock_ishock_node.cxx
dbsk2d_shock_ishock_node_sptr.h

dbsk2d_shock_grouped_ishock_edge.h dbsk2d_shock_grouped_ishock_edge.cxx
dbsk2d_shock_grouped_ishock_edge_sptr.h

# ***********************
# Visual fragment classes
# ***********************

dbsk2d_shock_fragment.h    dbsk2d_shock_fragment.cxx
dbsk2d_shock_fragment_sptr.h

dbsk2d_visual_fragment.h   dbsk2d_visual_fragment.cxx

# ******************************
# Topological boundary classes
# *******************************

dbsk2d_bnd_vertex.h    dbsk2d_bnd_vertex_sptr.h     dbsk2d_bnd_vertex.cxx
dbsk2d_bnd_edge.h      dbsk2d_bnd_edge_sptr.h       dbsk2d_bnd_edge.cxx    
dbsk2d_bnd_contour.h   dbsk2d_bnd_contour_sptr.h    dbsk2d_bnd_contour.cxx

dbsk2d_boundary.h                 dbsk2d_boundary.cxx
dbsk2d_boundary_sptr.h
#                                  dbsk2d_boundary_belm_iterator.cxx
dbsk2d_bnd_cell.h      dbsk2d_bnd_cell_sptr.h       dbsk2d_bnd_cell.cxx
dbsk2d_bnd_utils.h     dbsk2d_bnd_utils.cxx
                                  
# *********************************************************
# Basic boundary classes:
# Geometry classes used by the shock computation algorithm 
# *********************************************************

dbsk2d_ishock_bnd_key.h           dbsk2d_ishock_bnd_key.cxx                        
dbsk2d_ishock_belm.h              dbsk2d_ishock_belm.cxx
                                                                
dbsk2d_ishock_bpoint.h            dbsk2d_ishock_bpoint.cxx
dbsk2d_ishock_bcurve.h            dbsk2d_ishock_bcurve.cxx
dbsk2d_ishock_bline.h             dbsk2d_ishock_bline.cxx 
dbsk2d_ishock_barc.h              dbsk2d_ishock_barc.cxx  


# ***********************
# Intrinsic shock graph classes
# ***********************

dbsk2d_ishock_utils.h

# Shock graph data structures 
dbsk2d_ishock_elm.h                                
dbsk2d_ishock_node.h               dbsk2d_ishock_node.cxx               
dbsk2d_ishock_edge.h               dbsk2d_ishock_edge.cxx

dbsk2d_ishock_graph_sptr.h
dbsk2d_ishock_graph.h              dbsk2d_ishock_graph.cxx     
                                                                      
# Intrinsic shock edges          
dbsk2d_ishock_contact.h              dbsk2d_ishock_contact.cxx
#dbsk2d_ishock_pointline_contact.h   dbsk2d_ishock_pointline_contact.cxx  
#dbsk2d_ishock_pointarc_contact.h    dbsk2d_ishock_pointarc_contact.cxx   
dbsk2d_ishock_pointpoint.h           dbsk2d_ishock_pointpoint.cxx         
dbsk2d_ishock_pointline.h            dbsk2d_ishock_pointline.cxx          
dbsk2d_ishock_pointarc.h             dbsk2d_ishock_pointarc.cxx           
dbsk2d_ishock_lineline.h             dbsk2d_ishock_lineline.cxx           
dbsk2d_ishock_linearc.h              dbsk2d_ishock_linearc.cxx            
dbsk2d_ishock_arcarc.h               dbsk2d_ishock_arcarc.cxx             
dbsk2d_ishock_lineline_thirdorder.h  dbsk2d_ishock_lineline_thirdorder.cxx
dbsk2d_ishock_pointarc_thirdorder.h  dbsk2d_ishock_pointarc_thirdorder.cxx
dbsk2d_ishock_arcarc_thirdorder.h    dbsk2d_ishock_arcarc_thirdorder.cxx  

# *********************************************************
# Extrinsic Shock classes
# *********************************************************

dbsk2d_xshock_sample.h     dbsk2d_xshock_sample_sptr.h
dbsk2d_xshock_node.h       dbsk2d_xshock_node.cxx
dbsk2d_xshock_edge.h       dbsk2d_xshock_edge.cxx      
dbsk2d_xshock_edge_sptr.h

# *********************************************************
# Rich Map
# *********************************************************

dbsk2d_rich_map.h          dbsk2d_rich_map.cxx
dbsk2d_rich_map_sptr.h

# *********************************************************
# data structure for shock computation
# *********************************************************

dbsk2d_lagrangian_cell_bnd.h           dbsk2d_lagrangian_cell_bnd.cxx
dbsk2d_lagrangian_cell_bnd_sptr.h
dbsk2d_containment_node.h dbsk2d_containment_node.cxx
dbsk2d_containment_graph.h dbsk2d_containment_graph.cxx
dbsk2d_containment_node_sptr.h
dbsk2d_transform_manager.h dbsk2d_transform_manager.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbsk2d_sources)

ADD_LIBRARY(dbsk2d ${dbsk2d_sources})

TARGET_LINK_LIBRARIES( dbsk2d dbgrl vgl vcl vnl vbl vtol dbsol dbsol_algo )

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

# Shock algorithm library
SUBDIRS ( algo )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

