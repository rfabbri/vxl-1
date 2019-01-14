# contrib/Manifold_extraction/CMakeLists.txt
# Developement for : pradeep

include_directories( ${LEMS_SOURCE_DIR}/breye)
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec)
include_directories( ${LEMS_SOURCE_DIR}/contrib)
include_directories( ${LEMS_SOURCE_DIR}/basic)
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_CORE_INCLUDE_DIR} )


set(manifold_extraction_sources

vehicle_model.cxx                       vehicle_model.h  
vehicle_model_3d.cxx                    vehicle_model_3d.h
                                       vrml_print.h                                     
Lie_group_operations.cxx               Lie_group_operations.h 
Lie_contour_match.cxx                  Lie_contour_match.h
                                       Lie_contour_match_sptr.h
Lie_cv_cor.cxx                         Lie_cv_cor.h 
                                       Lie_cv_cor_sptr.h
Lie_contour_utilities.cxx              Lie_contour_utilities.h
Lie_spoke_utilities.cxx                Lie_spoke_utilities.h
)           

add_subdirectory(cmd)

add_subdirectory(Templates)

add_subdirectory(vis)

add_subdirectory(pro)

aux_source_directory(Templates manifold_extraction_sources)

add_library(manifold_extraction ${manifold_extraction_sources})


target_link_libraries(manifold_extraction vidpro1 sdet bsta brip bugl bsol btol bdgl vsol vil vgl_algo vnl vsl vbl_io vbl vul)













