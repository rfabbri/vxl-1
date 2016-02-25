INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

LINK_DIRECTORIES(${CMAKE_VLFEAT_LIBS})


#SET( vox_compute_composite_fragments_sources
#  vox_compute_composite_fragments_params.h 
#  vox_compute_composite_fragments_params_sptr.h
#  vox_compute_composite_fragments_params.cxx
#  vox_compute_composite_fragments_main.cxx
#  vbl_smart_ptr+vox_compute_composite_fragments_params-.cxx
#)

SET( vox_compute_composite_fragments_dbsk2d_sources
  vox_compute_composite_fragments_params.h 
  vox_compute_composite_fragments_params_sptr.h
  vox_compute_composite_fragments_params.cxx
  vox_compute_composite_fragments_dbsk2d_main.cxx
  vbl_smart_ptr+vox_compute_composite_fragments_params-.cxx
)

#ADD_EXECUTABLE (vox_compute_composite_fragments ${vox_compute_composite_fragments_sources} )
#TARGET_LINK_LIBRARIES (vox_compute_composite_fragments dbskfg dborl_algo dbsk2d_pro dbskfg_pro dbskfg_algo vidpro1 con_graph_pro)

ADD_EXECUTABLE (vox_compute_composite_dbsk2d_fragments ${vox_compute_composite_fragments_dbsk2d_sources} )
TARGET_LINK_LIBRARIES (vox_compute_composite_dbsk2d_fragments dbsk2d_pro dbsk2d dbsk2d_algo dbsk2d_algo dborl_algo dbsk2d_pro vidpro1 bsta vil3d )#dbskfg dbskfg_pro dbskfg_algo vidpro1 con_graph_pro)



