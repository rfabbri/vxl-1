INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET( dborl_eval_edge_dets_sources
  dborl_eval_edge_dets_params.h 
  dborl_eval_edge_dets_params_sptr.h
  dborl_eval_edge_dets_main.cxx
  vbl_smart_ptr+dborl_eval_edge_dets_params-.cxx
)

ADD_EXECUTABLE (dborl_eval_edge_dets ${dborl_eval_edge_dets_sources})
TARGET_LINK_LIBRARIES (dborl_eval_edge_dets dborl_algo dbdet_algo)

SET( dborl_eval_gen_edge_linker_sources
  dborl_eval_gen_edge_linker_params.h 
  dborl_eval_gen_edge_linker_params_sptr.h
  dborl_eval_gen_edge_linker_main.cxx
  vbl_smart_ptr+dborl_eval_gen_edge_linker_params-.cxx
)

ADD_EXECUTABLE (dborl_eval_gen_edge_linker ${dborl_eval_gen_edge_linker_sources})
TARGET_LINK_LIBRARIES (dborl_eval_gen_edge_linker dborl_algo dbdet_algo)


SET( dborl_eval_sel_edge_linker_sources
  dborl_eval_sel_edge_linker_params.h 
  dborl_eval_sel_edge_linker_params_sptr.h
  dborl_eval_sel_edge_linker_main.cxx
  vbl_smart_ptr+dborl_eval_sel_edge_linker_params-.cxx
)

ADD_EXECUTABLE (dborl_eval_sel_edge_linker ${dborl_eval_sel_edge_linker_sources})
TARGET_LINK_LIBRARIES (dborl_eval_sel_edge_linker dborl_algo dbdet_algo)

SET( dborl_eval_VD_sources
  dborl_eval_VD_params.h 
  dborl_eval_VD_params_sptr.h
  dborl_eval_VD_main.cxx
  vbl_smart_ptr+dborl_eval_VD_params-.cxx
)

ADD_EXECUTABLE (dborl_eval_VD ${dborl_eval_VD_sources})
TARGET_LINK_LIBRARIES (dborl_eval_VD dborl_algo sdet dbdet_algo)


