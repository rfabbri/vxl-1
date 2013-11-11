# Brown Curve Detection and Generation Library
# algo/bcdg/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )

SET(bcdg_sources
  bcdg_algo0.cxx              bcdg_algo0.h            
  bcdg_finger.cxx             bcdg_finger.h           bcdg_finger_sptr.h
  bcdg_frame.cxx              bcdg_frame.h            bcdg_frame_sptr.h
  bcdg_global_option.cxx      bcdg_global_option.h    bcdg_global_option_sptr.h
  bcdg_hypothel.cxx           bcdg_hypothel.h         bcdg_hypothel_sptr.h
  bcdg_hypothesis.h                                   bcdg_hypothesis_sptr.h
  bcdg_local_option.cxx       bcdg_local_option.h     bcdg_local_option_sptr.h
  bcdg_point_projection.cxx   bcdg_point_projection.h
  bcdg_3d_reconstructor.cxx   bcdg_3d_reconstructor.h
)

AUX_SOURCE_DIRECTORY(Templates bcdg_sources)

ADD_LIBRARY(bcdg ${bcdg_sources})

TARGET_LINK_LIBRARIES(bcdg vgl vbl vul vnl dbecl vsol) 

IF(BUILD_TESTING)
  #SUBDIRS(tests)
ENDIF(BUILD_TESTING)
