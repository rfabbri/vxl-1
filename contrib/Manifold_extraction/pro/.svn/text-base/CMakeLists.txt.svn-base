# contrib/Manifold_extraction/pro/CMakeLists.txt
# Developement for : pradeep

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )


SET(manifold_extraction_pro_sources

Lie_contour_match_process.cxx                  Lie_contour_match_process.h
Lie_contour_match_storage.cxx                  Lie_contour_match_storage.h
                                               Lie_contour_match_storage_sptr.h
shape_articulation_process.cxx                 shape_articulation_process.h       
Lie_contour_mean_process.cxx                   Lie_contour_mean_process.h
Lie_contour_discrete_mean_process.cxx          Lie_contour_discrete_mean_process.h
Lie_contour_geodesic_process.cxx               Lie_contour_geodesic_process.h
Lie_contour_principal_geodesic_shapes.cxx      Lie_contour_principal_geodesic_shapes.h
Lie_contour_generate_geodesic_shapes.cxx       Lie_contour_generate_geodesic_shapes.h
Lie_contour_classify_shapes.cxx                Lie_contour_classify_shapes.h
Lie_spoke_mean_process.cxx                     Lie_spoke_mean_process.h
Lie_spoke_geodesic_process.cxx                 Lie_spoke_geodesic_process.h
Lie_spoke_principal_geodesic_shapes.cxx        Lie_spoke_principal_geodesic_shapes.h
Lie_spoke_classify_shapes.cxx                  Lie_spoke_classify_shapes.h
Flip_spoke_process.cxx                         Flip_spoke_process.h 

)


ADD_LIBRARY(manifold_extraction_pro ${manifold_extraction_pro_sources})


TARGET_LINK_LIBRARIES(manifold_extraction_pro manifold_extraction vidpro1 brip vtol dbsol_algo dbsk2d_pro )













