# This is brcv/rec/dbskr/pro/CMakeLists.txt
# Brown shock recognition library

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ozge )

SET(dbskr_pro_sources

# storage classes
   dbskr_shock_match_storage.h                dbskr_shock_match_storage.cxx
   dbskr_shock_match_storage_sptr.h
   dbskr_shock_patch_storage.h                dbskr_shock_patch_storage.cxx
   dbskr_shock_patch_storage_sptr.h

   dbskr_shock_patch_match_storage.h          dbskr_shock_patch_match_storage.cxx
   dbskr_shock_patch_match_storage_sptr.h

# processes
   dbskr_compute_sk_path_curve_process.h      dbskr_compute_sk_path_curve_process.cxx
   dbskr_shock_match_process.h                dbskr_shock_match_process.cxx
   dbskr_shock_patch_match_process.h          dbskr_shock_patch_match_process.cxx
   dbskr_shock_patch_curve_match_process.h    dbskr_shock_patch_curve_match_process.cxx
   dbskr_subshock_match_process.h             dbskr_subshock_match_process.cxx
   #dbskr_shock_match_con_process.h           dbskr_shock_match_con_process.cxx
   dbskr_shock_match_pmi_process.h            dbskr_shock_match_pmi_process.cxx
   dbskr_extract_shock_patches_process.h      dbskr_extract_shock_patches_process.cxx
   dbskr_load_shock_patch_process.h           dbskr_load_shock_patch_process.cxx
   dbskr_save_shock_patch_process.h           dbskr_save_shock_patch_process.cxx
   dbskr_extract_subgraph_and_find_shock_patches_process.h   dbskr_extract_subgraph_and_find_shock_patches_process.cxx
   dbskr_detect_shock_patches_process.h       dbskr_detect_shock_patches_process.cxx 
)

ADD_LIBRARY(dbskr_pro ${dbskr_pro_sources})

#TARGET_LINK_LIBRARIES(dbskr_pro dbskr dbru strk vidpro1 vul dbsdef)

TARGET_LINK_LIBRARIES(dbskr_pro dbskr_algo strk bpro1 vul)


