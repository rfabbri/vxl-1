# This is brcv/shp/dbsk2d/pro/CMakeLists.txt
# Brown 2D Shock library

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbsk2d_pro_sources

# storage classes
   dbsk2d_shock_storage.h              dbsk2d_shock_storage.cxx
   dbsk2d_shock_storage_sptr.h

# processes
   dbsk2d_compute_ishock_process.h      dbsk2d_compute_ishock_process.cxx
   dbsk2d_prune_ishock_process.h        dbsk2d_prune_ishock_process.cxx
   dbsk2d_gap_transform_process.h       dbsk2d_gap_transform_process.cxx
   dbsk2d_sample_ishock_process.h       dbsk2d_sample_ishock_process.cxx
   
   dbsk2d_compile_rich_map_process.h    dbsk2d_compile_rich_map_process.cxx
   dbsk2d_assign_image_process.h        dbsk2d_assign_image_process.cxx
   
   dbsk2d_load_bnd_process.h            dbsk2d_load_bnd_process.cxx
   dbsk2d_save_bnd_process.h            dbsk2d_save_bnd_process.cxx
   dbsk2d_load_esf_process.h            dbsk2d_load_esf_process.cxx
   dbsk2d_save_esf_process.h            dbsk2d_save_esf_process.cxx
   dbsk2d_compute_containment_graph_process.h dbsk2d_compute_containment_graph_process.cxx
)

ADD_LIBRARY(dbsk2d_pro ${dbsk2d_pro_sources})

TARGET_LINK_LIBRARIES(dbsk2d_pro dbsk2d_algo bpro1 vul)


