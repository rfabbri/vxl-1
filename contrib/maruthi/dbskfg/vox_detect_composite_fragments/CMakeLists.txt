INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )


LINK_DIRECTORIES(${CMAKE_VLFEAT_LIBS})

SET( vox_detect_composite_fragments_sources
  vox_detect_composite_fragments_params.h 
  vox_detect_composite_fragments_params_sptr.h
  vox_detect_composite_fragments_params.cxx
  vox_detect_composite_fragments_main.cxx
  vbl_smart_ptr+vox_detect_composite_fragments_params-.cxx
)

ADD_EXECUTABLE (vox_detect_composite_fragments ${vox_detect_composite_fragments_sources} )
TARGET_LINK_LIBRARIES (vox_detect_composite_fragments dborl_algo dbskfg_algo dbskfg_pro vidpro1 dbxml)



