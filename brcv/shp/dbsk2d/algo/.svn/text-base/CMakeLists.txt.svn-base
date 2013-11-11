# This is brcv/shp/dbsk2d/algo/CMakeLists.txt
# The Development Brown Shock 2D Library - Specific Algorithms

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )


SET( dbsk2d_algo_sources

# *********************************************************
#             Various Algorithms
# *********************************************************


# *********************************************************
#             Utils (simplified usage) 
# *********************************************************
dbsk2d_compute_shocks.h                dbsk2d_compute_shocks.cxx
dbsk2d_compute_bounding_box.h          dbsk2d_compute_bounding_box.cxx
dbsk2d_extract_subgraph.h              dbsk2d_extract_subgraph.cxx
dbsk2d_hor_flip_shock_graph.h          dbsk2d_hor_flip_shock_graph.cxx

# *********************************************************
#             Boundary Preprocessing 
# *********************************************************
 
  dbsk2d_bnd_preprocess.h              dbsk2d_bnd_preprocess.cxx
                                       dbsk2d_bnd_preprocess_common.cxx
                                       dbsk2d_bnd_preprocess_points.cxx
                                       dbsk2d_bnd_preprocess_lines.cxx
                                       dbsk2d_bnd_preprocess_arcs.cxx
                                       dbsk2d_bnd_preprocess_lines_vs_arcs.cxx
  #dbsk2d_bnd_edge_intersect_profile.h  dbsk2d_bnd_edge_intersect_profile.cxx  
  

# *********************************************************
#           Shock computation algorithms
# *********************************************************


# *********************************************************
#  1. Standard Lagrangian shock detector
# *********************************************************

dbsk2d_ishock_intersection_data.h
dbsk2d_ishock_compute_intersection.h   dbsk2d_ishock_compute_intersection.cxx

dbsk2d_lagrangian_ishock_detector.h    dbsk2d_lagrangian_ishock_detector.cxx
                                       dbsk2d_lagrangian_ishock_detector_init.cxx
                                       dbsk2d_lagrangian_ishock_detector_prop.cxx

# *********************************************************
#  2. Eulerian-Lagrangian shock detector           
# *********************************************************
                            
dbsk2d_ishock_detector.h               dbsk2d_ishock_detector.cxx


# *********************************************************
#  3. Shock pruning
# *********************************************************

dbsk2d_prune_ishock.h                  dbsk2d_prune_ishock.cxx

# *********************************************************
#  3. Shock transforms
# *********************************************************

dbsk2d_shock_transforms.h              dbsk2d_shock_transforms.cxx
dbsk2d_ishock_transform.h             dbsk2d_ishock_transform.cxx
dbsk2d_ishock_transform_sptr.h
dbsk2d_ishock_loop_transform.h        dbsk2d_ishock_loop_transform.cxx
dbsk2d_ishock_gap_transform.h        dbsk2d_ishock_gap_transform.cxx
dbsk2d_ishock_gap4_transform.h        dbsk2d_ishock_gap4_transform.cxx
dbsk2d_ishock_gap_detector.h         dbsk2d_ishock_gap_detector.cxx
dbsk2d_ishock_grouping_transform.h dbsk2d_ishock_grouping_transform.cxx
# *********************************************************
# * 4. Sample intrinsic shock
# *********************************************************

dbsk2d_sample_ishock.h  dbsk2d_sample_ishock.cxx

# *********************************************************
#  5. Extrinsic Shock File (ESF) I/O
# *********************************************************

dbsk2d_xshock_graph_fileio.h          dbsk2d_xshock_graph_fileio.cxx

# *********************************************************
#  6.Symmetry transform Algorithms
# *********************************************************

# *********************************************************
#  7. SCDM
# *********************************************************

#dbsk2d_scdm.h                     dbsk2d_scdm.cxx

# *********************************************************
#  8. CEDT
# *********************************************************

#dbsk2d_cedt.h                     dbsk2d_cedt.cxx

# *********************************************************
#  9. Curve algorithms
# *********************************************************

#dbsk2d_ishock_curve_utils.h       dbsk2d_ishock_curve_utils.cxx

dbsk2d_xshock_graph_loader_from_esg_sh_file.h dbsk2d_xshock_graph_loader_from_esg_sh_file.cxx

)

ADD_LIBRARY( dbsk2d_algo  ${dbsk2d_algo_sources} )

TARGET_LINK_LIBRARIES( dbsk2d_algo dbsk2d )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )



