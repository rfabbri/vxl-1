# Brown affine patch library
# algo/bapl/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SET(bapl_sources
   bapl_keypoint.cxx            bapl_keypoint.h                 bapl_keypoint_sptr.h
   bapl_lowe_keypoint.cxx       bapl_lowe_keypoint.h            bapl_lowe_keypoint_sptr.h
   bapl_lowe_pyramid.txx        bapl_lowe_pyramid.h
   bapl_lowe_pyramid_set.cxx    bapl_lowe_pyramid_set.h         bapl_lowe_pyramid_set_sptr.h
   bapl_keypoint_extractor.cxx  bapl_keypoint_extractor.h
   bapl_bbf_tree.cxx            bapl_bbf_tree.h
   bapl_lowe_cluster.cxx        bapl_lowe_cluster.h
   bapl_affine2d_est.cxx        bapl_affine2d_est.h
   bapl_affine_transform.h      bapl_affine_transform.cxx
   bapl_affine_roi.h            bapl_affine_roi.cxx
   bapl_mi_matcher.h            bapl_mi_matcher.cxx
   bapl_mi_matcher_params.h     bapl_mi_matcher_params.cxx
)

AUX_SOURCE_DIRECTORY(Templates bapl_sources)

ADD_LIBRARY(bapl  ${bapl_sources})

TARGET_LINK_LIBRARIES(bapl vnl vbl vil ipts vimt brip rrel)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )


IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

