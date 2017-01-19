# Brown Shock Recognition Library  
# brcv/rec/dbskr/CMakeLists.txt

include_directories(${VXL_MUL_INCLUDE_DIR})
include_directories(${VXL_GEL_INCLUDE_DIR})
include_directories(${VXL_GEL_INCLUDE_DIR}/mrc)
include_directories(${VXL_BRL_INCLUDE_DIR})
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
include_directories(${lems_SOURCE_DIR}/bmvgd/bmvl )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg)
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR} )

set(dbskr_sources
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

aux_source_directory(Templates dbskr_sources)

add_library(dbskr  ${dbskr_sources})

target_link_libraries(dbskr dbsk2d_algo vnl bsold_algo dbcvr dbru dbdet_algo rgrl)
#target_link_libraries(dbskr dbsk2d_algo vnl bsold_algo dbcvr dbru dbdet_algo brct rgrl)

# algorithms library
add_subdirectory(algo)

# Processing Library
add_subdirectory(pro)

# Visualization Library
add_subdirectory(vis)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

# Executables
add_subdirectory(bin)
