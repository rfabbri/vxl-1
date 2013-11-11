# Brown Shock Recognition Library  
# brcv/rec/dbskr/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bmvl)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )

SET(dbskr_sources
dbskr_tree.h                dbskr_tree.cxx
dbskr_tree_sptr.h
#dbskr_tree_edit_mi.h       dbskr_tree_edit_mi.cxx
#dbskr_te_acost.h           dbskr_te_acost.cxx
dbskr_tree_edit.h           dbskr_tree_edit.cxx
dbskr_tree_edit_pmi.h       dbskr_tree_edit_pmi.cxx
dbskr_tree_edit_coarse.h    dbskr_tree_edit_coarse.cxx
dbskr_tree_edit_combined.h  dbskr_tree_edit_combined.cxx

dbskr_scurve.h         dbskr_scurve.cxx
dbskr_scurve_sptr.h

dbskr_sc_pair.h             dbskr_sc_pair.cxx
dbskr_sc_pair_sptr.h

dbskr_sm_cor.h         dbskr_sm_cor.cxx
dbskr_sm_cor_sptr.h



dbskr_compute_scurve.h  dbskr_compute_scurve.cxx
dbskr_dpmatch.h         dbskr_dpmatch.cxx
dbskr_dpmatch_combined.h    dbskr_dpmatch_combined.cxx
dbskr_localize_match.h  dbskr_localize_match.cxx

dbskr_shock_patch.h     dbskr_shock_patch.cxx
dbskr_shock_patch_sptr.h
dbskr_v_graph_sptr.h

dbskr_utilities.h dbskr_utilities.cxx

# Edit-distance for shock graphs
  dbskr_directed_tree.h                              dbskr_directed_tree.cxx
  dbskr_directed_tree_sptr.h                         
  dbskr_shock_tree.h                                 dbskr_shock_tree.cxx
  dbskr_shock_tree_sptr.h                            
  dbskr_edit_distance_base.h                         dbskr_edit_distance_base.cxx
  dbskr_edit_distance_shock_tree.h                   dbskr_edit_distance_shock_tree.cxx

)

AUX_SOURCE_DIRECTORY(Templates dbskr_sources)

ADD_LIBRARY(dbskr  ${dbskr_sources})

TARGET_LINK_LIBRARIES(dbskr dbsk2d_algo vnl dbsol_algo dbcvr dbru dbdet_algo rgrl)
#TARGET_LINK_LIBRARIES(dbskr dbsk2d_algo vnl dbsol_algo dbcvr dbru dbdet_algo brct rgrl)

# algorithms library
SUBDIRS(algo)

# Processing Library
SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

# Executables
SUBDIRS(bin)
