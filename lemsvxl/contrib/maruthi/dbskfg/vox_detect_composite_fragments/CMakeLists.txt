include_directories( ${LEMS_SOURCE_DIR}/db3p )


link_directories(${CMAKE_VLFEAT_LIBS})

set( vox_detect_composite_fragments_sources
  vox_detect_composite_fragments_params.h 
  vox_detect_composite_fragments_params_sptr.h
  vox_detect_composite_fragments_params.cxx
  vox_detect_composite_fragments_main.cxx
  vbl_smart_ptr+vox_detect_composite_fragments_params-.cxx
)

add_executable (vox_detect_composite_fragments ${vox_detect_composite_fragments_sources} )
target_link_libraries (vox_detect_composite_fragments dborl_algo dbskfg_algo dbskfg_pro vidpro1 dbxml)



