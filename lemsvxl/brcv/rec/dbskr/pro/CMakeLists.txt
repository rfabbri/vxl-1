# This is brcv/rec/dbskr/pro/CMakeLists.txt
# Brown shock recognition library

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/breye1 )

include_directories( ${lems_SOURCE_DIR}/contrib/ozge )

set(dbskr_pro_sources
# further algorithms beyond algo/ that depend on pro/
   dbskr_shock_patch_selector.h dbskr_shock_patch_selector.cxx
   dbskr_shock_patch_model_selector.h dbskr_shock_patch_model_selector.cxx

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

add_library(dbskr_pro ${dbskr_pro_sources})

#target_link_libraries(dbskr_pro dbskr dbru strk vidpro1 vul dbsdef)

target_link_libraries(dbskr_pro dbskr_algo bpro1 vul)


