# This is dbrl/dbseg/dbbgm/CMakeLists.txt
# Brown background modeling Library

doxygen_add_library(contrib/dbrl/dbseg/dbbgmo
  DEPENDS contrib/dbrl/dbbas/dbsta core/vil core/vsl
  PACKAGE contrib/dbrl
  )

SET(dbbgmo_sources

  bbgm_planes_to_sample.h
  bbgm_update.h
  bbgm_apply.h
  bbgm_detect.h
  bbgm_image_of.h         bbgm_image_of.cxx      bbgm_image_of.txx  bbgm_image_sptr.h
  bbgm_viewer.h           bbgm_viewer.cxx        bbgm_viewer_sptr.h
  bbgm_view_maker.h                              bbgm_view_maker_sptr.h
  bbgm_loader.h           bbgm_loader.cxx
  bbgm_measure.h
  bbgm_feature_image.h    bbgm_feature_image.cxx bbgm_feature_image.txx
  bbgm_feature_image_sptr.h
  bbgm_features.h         bbgm_features.cxx
  bbgm_wavelet_compressor.h bbgm_wavelet_compressor.cxx
   
  bbgm_vil_wavelet.h bbgm_vil_wavelet.txx
  bbgm_wavelet_image_traits.h 	
  bbgm_wavelet.h bbgm_wavelet.txx  bbgm_wavelet.cxx  bbgm_wavelet_sptr.h
  bbgm_wavelet_engine_base.h
  bbgm_wavelet_engine_GE.h  bbgm_wavelet_engine_GE.txx
  bbgm_wavelet_engine_kdu.h  bbgm_wavelet_engine_kdu_traits.h
  bbgm_interpolate.h
)
#TODO enclose statement with LINK_WITH_DLIBS if BRIP will be ported
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)


AUX_SOURCE_DIRECTORY(Templates dbbgmo_sources)
ADD_LIBRARY(dbbgmo  ${dbbgmo_sources})

# add the required libraries into this list
#switch to linking with the main brl root using LINK_WITH_DLIBS
IF (LINK_WITH_DLIBS)
	add_definitions( 
	-DDEVELINCLUDEPATH
	)
	TARGET_LINK_LIBRARIES(dbbgmo dbstao dbstao_algo brip vnl_io vbl_io vsl vnl vil_algo vil vgl_algo vgl vbl 
	kdu_coresys kdu_background_comp kdu_background_exp)
	IF (BUILD_OCTI_TESTING OR BUILD_TESTING)
	SUBDIRS(tests)
	ENDIF(BUILD_OCTI_TESTING OR BUILD_TESTING)
	ELSE(LINK_WITH_DLIBS)
	INCLUDE_DIRECTORIES(${brl_SOURCE_DIR}/bbas)
	TARGET_LINK_LIBRARIES(dbbgmo bsta bsta_algo brip vnl_io vbl_io vsl vnl vil_algo vil vgl_algo vgl vbl)
	
ENDIF(LINK_WITH_DLIBS)


SUBDIRS(pro)
SUBDIRS(vis)


