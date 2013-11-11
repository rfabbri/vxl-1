INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )

SET( georegister_sources
      dbrl_feature_sptr.h dbrl_feature.h        dbrl_feature.cxx
      dbrl_feature_point.h      dbrl_feature_point.cxx dbrl_feature_point_sptr.h
      dbrl_feature_point_tangent.h      dbrl_feature_point_tangent.cxx dbrl_feature_point_tangent_sptr.h 
      dbrl_feature_point_tangent_curvature.h dbrl_feature_point_tangent_curvature.cxx
      
      dbrl_transformation.h     dbrl_transformation_sptr.h dbrl_transformation.cxx
      dbrl_translation.h        dbrl_translation.cxx 
      dbrl_rigid_transformation.h dbrl_rigid_transformation.cxx
      dbrl_affine_transformation.h dbrl_affine_transformation.cxx
      dbrl_cubic_transformation.h dbrl_cubic_transformation.cxx

      dbrl_thin_plate_spline_transformation.h       dbrl_thin_plate_spline_transformation.cxx
      dbrl_match_set.h      dbrl_match_set.cxx      dbrl_match_set_sptr.h
      dbrl_estimator.h      dbrl_estimator_sptr.h dbrl_estimator.cxx
      dbrl_estimator_point_rigid.h  
      dbrl_estimator_point_affine.h dbrl_estimator_point_affine.cxx
      dbrl_estimator_thin_plate_spline.h dbrl_estimator_thin_plate_spline.cxx
      dbrl_estimator_cubic_patch.h dbrl_estimator_cubic_patch.cxx
      dbrl_correspondence.h  dbrl_correspondence.cxx
      
      dbrl_robust_point_matching.h  dbrl_robust_point_matching.cxx
      dbrl_rpm_rigid.h
      dbrl_rpm_affine.h     dbrl_rpm_affine.cxx
      dbrl_rpm_tps.h        dbrl_rpm_tps.cxx
      
      dbrl_id_point_2d.h    dbrl_id_point_2d_sptr.h dbrl_id_point_2d.cxx
      dbrl_support.h        dbrl_support.cxx

#      dbrl_multiple_instance_object.h dbrl_multiple_instance_object.cxx dbrl_multiple_instance_object_sptr.h
#      moved to brcv/rec/dbru

      dbrl_feature_point_tangent_curvature_groupings.h dbrl_feature_point_tangent_curvature_groupings.cxx
)
AUX_SOURCE_DIRECTORY(Templates georegister_sources)
ADD_LIBRARY(georegister  ${georegister_sources})

TARGET_LINK_LIBRARIES(georegister vbl dbsta_algo dbnl_algo vul dt vil_io dbgl_algo vgui)

#tests
SUBDIRS(tests)
