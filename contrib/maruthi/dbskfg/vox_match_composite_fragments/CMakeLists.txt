include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

link_directories(${CMAKE_VLFEAT_LIBS})


set( vox_match_composite_fragments_sources
  vox_match_composite_fragments_params.h 
  vox_match_composite_fragments_params_sptr.h
#  vox_match_bag_of_fragments.h
#  vox_match_bag_of_fragments.cxx
  vox_match_composite_fragments_params.cxx
  vox_match_composite_fragments_main.cxx
  vbl_smart_ptr+vox_match_composite_fragments_params-.cxx
)

add_executable (vox_match_composite_fragments ${vox_match_composite_fragments_sources} )
target_link_libraries (vox_match_composite_fragments dbskfg dborl_algo dbsk2d_pro dbskfg_pro dbskfg_algo dbdet_pro )



