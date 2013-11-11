INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VTK_LIBRARY_PATH})
INCLUDE_DIRECTORIES( ${VTK_INCLUDE_PATH})

SET(elbow_storage_sources
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
    
ADD_LIBRARY (elbow_storage ${elbow_storage_sources})
TARGET_LINK_LIBRARIES (elbow_storage bpro1 vil3d)