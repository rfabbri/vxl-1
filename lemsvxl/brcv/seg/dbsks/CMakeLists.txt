# Brown Symmetry-based segmentation library
# seg/dbsks

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_CONVERSIONS_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/breye1)
include_directories( ${LEMS_SOURCE_DIR}/basic)
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories(${VXD_BRLD_INCLUDE_DIR})


set( dbsks_sources

dbsks_detect_symmetry.h    dbsks_detect_symmetry.cxx

dbsks_dp_match.h     dbsks_dp_match.cxx              dbsks_dp_match_sptr.h
dbsks_circ_arc_grid.h    dbsks_circ_arc_grid.cxx
dbsks_shapelet_grid.h    dbsks_shapelet_grid.cxx

dbsks_ocm_image_cost.h   dbsks_ocm_image_cost.cxx    dbsks_ocm_image_cost_sptr.h
dbsks_dp_match_utils.h   dbsks_dp_match_utils.cxx
dbsks_utils.h            dbsks_utils.cxx

dbsks_local_match.h      dbsks_local_match.cxx       dbsks_local_match_sptr.h
dbsks_local_match_cost.h      dbsks_local_match_cost.cxx

dbsks_shock_graph_stats.h     dbsks_shock_graph_stats.cxx
dbsks_shapelet_stats.h        dbsks_shapelet_stats.cxx
dbsks_hog.h                   dbsks_hog.cxx

# Binary I/O
dbsks_io_matrix_array.h       dbsks_io_matrix_array.cxx
dbsks_io_circ_arc_grid.h      dbsks_io_circ_arc_grid.cxx
dbsks_io_shapelet_grid.h      dbsks_io_shapelet_grid.cxx


# SHOULD GO TO DBDET
dbdet_arc_patch.h       dbdet_arc_patch.cxx

# XSHOCK-based detection -------------------------------------------------------
dbsks_xfrag_bhog_model.cxx         dbsks_xfrag_bhog_model.h
dbsks_xshock_dp.cxx                dbsks_xshock_dp.h
dbsks_xshock_dp_new.cxx            dbsks_xshock_dp_new.h
dbsks_xshock_dp_helper.cxx         dbsks_xshock_dp_helper.h
dbsks_xnode_grid.cxx               dbsks_xnode_grid.h
dbsks_xshock_utils.cxx             dbsks_xshock_utils.h
dbsks_xfrag_geom_model.cxx         dbsks_xfrag_geom_model.h     dbsks_xfrag_geom_model_sptr.h
dbsks_xnode_geom.cxx               dbsks_xnode_geom.h           dbsks_xnode_geom_sptr.h
dbsks_xnode_geom_model.cxx         dbsks_xnode_geom_model.h     dbsks_xnode_geom_model_sptr.h
dbsks_xgraph_geom_model.cxx        dbsks_xgraph_geom_model.h    dbsks_xgraph_geom_model_sptr.h
dbsks_xgraph_ccm_model.cxx         dbsks_xgraph_ccm_model.h     dbsks_xgraph_ccm_model_sptr.h
dbsks_bfrag_cost_model.cxx         dbsks_bfrag_cost_model.h

dbsks_biarc_grid.cxx               dbsks_biarc_grid.h

# deleted files
#dbsks_xfrag_image_cost.cxx         dbsks_xfrag_image_cost.h
#dbsks_shotton_ocm.cxx              dbsks_shotton_ocm.h
#dbsks_xio_xshock.cxx               dbsks_xio_xshock.h



# depreciated files
#dbsks_ccm.cxx                      dbsks_ccm.h
#dbsks_wcm.cxx                      dbsks_wcm.h
#dbsks_xshock_wcm_likelihood.cxx    dbsks_xshock_wcm_likelihood.h
#dbsks_compute_ocm_cost.cxx         dbsks_compute_ocm_cost.h
#dbsks_gray_ocm.cxx                 dbsks_gray_ocm.h


dbsks_subpix_ccm.cxx               dbsks_subpix_ccm.h
dbsks_xshock_likelihood.cxx        dbsks_xshock_likelihood.h
dbsks_xshock_ccm_likelihood.cxx    dbsks_xshock_ccm_likelihood.h
dbsks_biarc_sampler.cxx            dbsks_biarc_sampler.h         dbsks_biarc_sampler_sptr.h                 
dbsks_det_desc_xgraph.cxx          dbsks_det_desc_xgraph.h       dbsks_det_desc_xgraph_sptr.h
dbsks_det_nms.cxx                  dbsks_det_nms.h
dbsks_xshock_det_record.cxx        dbsks_xshock_det_record.h

# end-user function
dbsks_xshock_detector.cxx          dbsks_xshock_detector.h



# LIBSVM - should go to db3p
libsvm/svm.h
libsvm/svm.cpp

)

aux_source_directory(Templates dbsks_sources)

add_library(dbsks ${dbsks_sources})

target_link_libraries(dbsks dbsksp_algo dborl dbdet_algo vidpro1 vnl_io)



if( BUILD_EXAMPLES )
  add_subdirectory( cmdline )
endif( BUILD_EXAMPLES )

# Algorithm library
add_subdirectory( algo )

# Processing Library
add_subdirectory( pro )

# Visualization Library
add_subdirectory( vis )

# XML I/O Library
add_subdirectory( xio )




if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
