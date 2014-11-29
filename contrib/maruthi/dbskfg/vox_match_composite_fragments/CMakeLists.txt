INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

LINK_DIRECTORIES(${CMAKE_VLFEAT_LIBS})


SET( vox_match_composite_fragments_sources
  vox_match_composite_fragments_params.h 
  vox_match_composite_fragments_params_sptr.h
#  vox_match_bag_of_fragments.h
#  vox_match_bag_of_fragments.cxx
  vox_match_composite_fragments_params.cxx
  vox_match_composite_fragments_main.cxx
  vbl_smart_ptr+vox_match_composite_fragments_params-.cxx
)

ADD_EXECUTABLE (vox_match_composite_fragments ${vox_match_composite_fragments_sources} )
TARGET_LINK_LIBRARIES (vox_match_composite_fragments dbskfg dborl_algo dbsk2d_pro dbskfg_pro dbskfg_algo dbdet_pro )



