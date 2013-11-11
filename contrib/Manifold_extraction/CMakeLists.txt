# contrib/Manifold_extraction/CMakeLists.txt
# Developement for : pradeep

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )


SET(manifold_extraction_sources

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

SUBDIRS(cmd)

SUBDIRS(Templates)

SUBDIRS(vis)

SUBDIRS(pro)

AUX_SOURCE_DIRECTORY(Templates manifold_extraction_sources)

ADD_LIBRARY(manifold_extraction ${manifold_extraction_sources})


TARGET_LINK_LIBRARIES(manifold_extraction vidpro1 sdet bsta brip bugl bsol btol bdgl vsol vil vgl_algo vnl vsl vbl_io vbl vul)













