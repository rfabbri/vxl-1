include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set( dborl_eval_edge_dets_sources
  dborl_eval_edge_dets_params.h 
  dborl_eval_edge_dets_params_sptr.h
  dborl_eval_edge_dets_main.cxx
  vbl_smart_ptr+dborl_eval_edge_dets_params-.cxx
)

add_executable (dborl_eval_edge_dets ${dborl_eval_edge_dets_sources})
target_link_libraries (dborl_eval_edge_dets dborl_algo dbdet_algo)

set( dborl_eval_gen_edge_linker_sources
  dborl_eval_gen_edge_linker_params.h 
  dborl_eval_gen_edge_linker_params_sptr.h
  dborl_eval_gen_edge_linker_main.cxx
  vbl_smart_ptr+dborl_eval_gen_edge_linker_params-.cxx
)

add_executable (dborl_eval_gen_edge_linker ${dborl_eval_gen_edge_linker_sources})
target_link_libraries (dborl_eval_gen_edge_linker dborl_algo dbdet_algo)


set( dborl_eval_sel_edge_linker_sources
  dborl_eval_sel_edge_linker_params.h 
  dborl_eval_sel_edge_linker_params_sptr.h
  dborl_eval_sel_edge_linker_main.cxx
  vbl_smart_ptr+dborl_eval_sel_edge_linker_params-.cxx
)

add_executable (dborl_eval_sel_edge_linker ${dborl_eval_sel_edge_linker_sources})
target_link_libraries (dborl_eval_sel_edge_linker dborl_algo dbdet_algo)

set( dborl_eval_VD_sources
  dborl_eval_VD_params.h 
  dborl_eval_VD_params_sptr.h
  dborl_eval_VD_main.cxx
  vbl_smart_ptr+dborl_eval_VD_params-.cxx
)

add_executable (dborl_eval_VD ${dborl_eval_VD_sources})
target_link_libraries (dborl_eval_VD dborl_algo sdet dbdet_algo)


