# brcv/rec/dborl/CMakeLists.txt
# Brown ORL - Object Recognition Library - web database to store datasets and cluster management via MPI

project( dborl )

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/b3p )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_EXPAT_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories(${VXD_BRLD_INCLUDE_DIR})


set(dborl_sources  

dborl_object_base.h                  #dborl_object_base.cxx
dborl_object_base_sptr.h

dborl_image_object.h                 dborl_image_object.cxx
dborl_image_object_sptr.h 

dborl_description_base.h
dborl_description_base_sptr.h

dborl_image_bbox_description.h         dborl_image_bbox_description.cxx
dborl_image_bbox_description_sptr.h

dborl_image_data_description_base.h    #dborl_image_data_description_base.cxx
dborl_image_data_description_base_sptr.h

dborl_image_description.h              dborl_image_description.cxx
dborl_image_description_sptr.h

dborl_image_mask_description.h         dborl_image_mask_description.cxx
dborl_image_mask_description_sptr.h

dborl_image_polygon_description.h      dborl_image_polygon_description.cxx
dborl_image_polygon_description_sptr.h

dborl_image_description_utils.h        dborl_image_description_utils.cxx

dborl_dataset.h                        dborl_dataset.cxx
dborl_dataset_sptr.h

dborl_category_info.h                  dborl_category_info.cxx
dborl_category_info_sptr.h

dborl_evaluation.h                     dborl_evaluation.cxx
dborl_exp_stat_sptr.h

dborl_index_node_base.h
dborl_index_node_base_sptr.h

dborl_index_node.h                     dborl_index_node.cxx
dborl_index_node_sptr.h

dborl_index_leaf.h                     dborl_index_leaf.cxx
dborl_index_leaf_sptr.h

dborl_index_edge.h           
dborl_index_edge_sptr.h

dborl_index.h                          dborl_index.cxx
dborl_index_sptr.h

dborl_parameter.h                      dborl_parameter.cxx
dborl_algo_params_base.h               dborl_algo_params_base.cxx
dborl_algo_params_base_sptr.h  

dborl_det_desc.h                       dborl_det_desc.cxx
dborl_det_desc_sptr.h
dborl_eval_det.h                       dborl_eval_det.cxx

dborl_index_query.h                    dborl_index_query.cxx

)

aux_source_directory(Templates dborl_sources)

add_library(dborl ${dborl_sources})

target_link_libraries(dborl bsol vsol bgrld bdgl vil vbl)

# Brown Recognition Tools to aid Testing
#add_subdirectory( vis )

# Brown Recognition Algorithms via MPI
add_subdirectory(algo)

# Brown ORL Testing
add_subdirectory( tests )

# Brown ORL Command Line Utilities
add_subdirectory( bin )

# Brown Python Interface
add_subdirectory( pro )

