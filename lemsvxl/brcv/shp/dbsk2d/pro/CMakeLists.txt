# This is brcv/shp/dbsk2d/pro/CMakeLists.txt
# Brown 2D Shock library

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/breye1 )

set(dbsk2d_pro_sources

# storage classes
   dbsk2d_shock_storage.h              dbsk2d_shock_storage.cxx
   dbsk2d_shock_storage_sptr.h

# further data structures and algorithms above algo/


  dbsk2d_containment_node.h dbsk2d_containment_node.cxx
  dbsk2d_containment_node_sptr.h
  dbsk2d_containment_graph.h dbsk2d_containment_graph.cxx
  dbsk2d_transform_manager.h dbsk2d_transform_manager.cxx

# further shock transforms above algo/
  dbsk2d_ishock_gap_transform.h        dbsk2d_ishock_gap_transform.cxx
  dbsk2d_ishock_grouping_transform.h dbsk2d_ishock_grouping_transform.cxx
  dbsk2d_ishock_gap4_transform.h        dbsk2d_ishock_gap4_transform.cxx
  dbsk2d_ishock_loop_transform.h        dbsk2d_ishock_loop_transform.cxx


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

add_library(dbsk2d_pro ${dbsk2d_pro_sources})

target_link_libraries(dbsk2d_pro dbsk2d_algo bpro1 vil1 vul)


