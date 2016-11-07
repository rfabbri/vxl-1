include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VTK_LIBRARY_PATH})
include_directories( ${VTK_INCLUDE_PATH})

set(elbow_storage_sources
       elbow_vil3d_storage.cxx
       elbow_vil3d_storage.h
       elbow_vil3d_storage_sptr.h
       elbow_mesh_file_storage.cxx
       elbow_mesh_file_storage.h
       elbow_mesh_file_storage_sptr.h
       dbdet_third_order_3d_edge_storage.h
       dbdet_third_order_3d_edge_storage.cxx
       dbdet_third_order_3d_edge_storage_sptr.h
       elbow_levelset_function_storage.cxx
       elbow_levelset_function_storage.h
       elbow_levelset_function_storage_sptr.h
    )
    
add_library (elbow_storage ${elbow_storage_sources})
target_link_libraries (elbow_storage bpro1 vil3d)