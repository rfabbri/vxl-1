# dbrl/dbseg/dboxm/opt/CMakeLists.txt

include_directories( ${GEL_INCLUDE_DIR}/mrc )

set(boxm_opt_sources
   boxm_aux_traits.h
   boxm_aux_scene.hxx                   boxm_aux_scene.h
   boxm_opt_rt_bayesian_optimizer.hxx   boxm_opt_rt_bayesian_optimizer.h
   boxm_opt_appearance_estimator.h
   boxm_raytrace_function.h
   boxm_generate_opt_sample_functor.h
   boxm_render_expected_depth_functor.h
   boxm_render_expected_image_functor.h
   boxm_seg_length_functor.h
   boxm_pre_infinity_functor.h
   boxm_rt_sample.h                     boxm_rt_sample.hxx
   boxm_update_image_functor.h
   boxm_edge_updater.h                  boxm_edge_updater.hxx
   boxm_generate_edge_sample_functor.h
   boxm_render_expected_edge_image_functor.h
   boxm_rpc_registration.h
)

aux_source_directory(Templates dboxm_opt_sources)

add_library(dboxm_opt ${dboxm_opt_sources})

target_link_libraries(dboxm_opt dboxm vpl)

#processes
#add_subdirectory(pro)

#tests
add_subdirectory(tests)

#open_cl parallel computation
add_subdirectory(open_cl)
