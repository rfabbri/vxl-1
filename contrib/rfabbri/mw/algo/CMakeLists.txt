IF (VXL_VGUI_FOUND)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
ENDIF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/rfabbri )

# Multiview algo - higher-level and end-application stuff

SET( mw_algo_sources
  mw_match_position_band.cxx      mw_match_position_band.h
  mw_match_tangent_band.cxx      mw_match_tangent_band.h
  mw_discrete_corresp_algo.txx  mw_discrete_corresp_algo_defs.cxx  mw_discrete_corresp_algo.h
  mw_match_old.cxx      mw_match_old.h
  mw_point_matcher.cxx         mw_point_matcher.h
  mw_stereo_app.cxx    mw_stereo_app.h
  mw_algo_util.cxx       mw_algo_util.h
  mw_data.cxx       mw_data.h
  dvpgl_DG_bundle_adjust.cxx dvpgl_DG_bundle_adjust.h
  mw_qualitative_epipolar.cxx mw_qualitative_epipolar.h
  mw_sift_curve_algo.cxx mw_sift_curve_algo.h
  siftpp/sift.cpp siftpp/sift.hpp siftpp/sift.ipp siftpp/sift-conv.tpp
  mw_curve_stereo.cxx mw_curve_stereo.h
  mw_dt_curve_stereo.cxx mw_dt_curve_stereo.h
  mw_odt_curve_stereo.cxx mw_odt_curve_stereo.h
  dvcpl_distmap_bundle_adjust.cxx dvcpl_distmap_bundle_adjust.h
  dvcpl_distmap_optimize_camera.cxx dvcpl_distmap_optimize_camera.h
  #  dg-estimation/mw_geometry_estimation.cxx dg-estimation/mw_geometry_estimation.h
  #  dg-estimation/mw_test_sel_geometry.cxx dg-estimation/mw_test_sel_geometry.h
  #  mw_cvmatch.cxx       mw_cvmatch.h mw_cvmatch_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates mw_sources)

ADD_LIBRARY(mw_algo ${mw_algo_sources})

TARGET_LINK_LIBRARIES(mw_algo mw vidpro1 bgui vil dbil_algo dbdet_pro dbgl_algo brct dvpgl_algo dbecl dbcsi dvpgl_io dbdif_algo dbspi dbul)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
