# Brown affine patch library
# algo/bapl/CMakeLists.txt

include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

set(bapl_sources
   bapl_keypoint.cxx            bapl_keypoint.h                 bapl_keypoint_sptr.h
   bapl_lowe_keypoint.cxx       bapl_lowe_keypoint.h            bapl_lowe_keypoint_sptr.h
   bapl_lowe_pyramid.hxx        bapl_lowe_pyramid.h
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

aux_source_directory(Templates bapl_sources)

add_library(bapl  ${bapl_sources})

target_link_libraries(bapl vnl vbl vil ipts vimt brip rrel)

if( BUILD_EXAMPLES )
  add_subdirectory( examples )
endif( BUILD_EXAMPLES )


if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

