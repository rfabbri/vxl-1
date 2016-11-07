# This is contrib/dec/psm/algo/CMakeLists.txt
# Probabalistic Scene Modeling Algorithms

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/contrib/dec )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
  
#if(PSM_USE_OPENGL2_1)
#  include_directories(${OPENGL_INCLUDE_DIR})
#  include_directories(${GLUT_INCLUDE_DIR})
#  include_directories(${GLEW_INCLUDE_DIR})
#endif(PSM_USE_OPENGL2_1)

set(psm_algo_sources
  psm_raytrace_operations.h psm_raytrace_operations.cxx
  psm_refine_scene.h
  psm_condense_scene.h
  psm_parallel_raytrace_function.h
  psm_track_rays_parallel.h
  psm_render_expected.h
  psm_render_expected_aa.h
  psm_render_expected_parallel.h
  psm_pixel_probability_density.h
  psm_pixel_probability_range.h
  psm_update_aa.h
  psm_update_parallel.h
  psm_update_2pass.h
  psm_update_2pass_aa.h
  psm_update_vis_implicit_parallel.h
  psm_update_vis_implicit_aa.h
  psm_cell_visibility_iterator.h psm_cell_visibility_iterator.hxx
  psm_block_visibility_iterator.h psm_block_visibility_iterator.hxx
  psm_image_pyramid.h
  psm_triangle_scan_iterator_aa.h  psm_triangle_scan_iterator_aa.cxx
  psm_triangle_scan_iterator.h     psm_triangle_scan_iterator.cxx
  psm_triangle_interpolation_iterator_aa.h  psm_triangle_interpolation_iterator_aa.hxx
  psm_triangle_interpolation_iterator.h  psm_triangle_interpolation_iterator.hxx
  psm_raytrace_function.h
  psm_convert_to_boct.h
  psm_clean_unobserved.h
  psm_compute_expected_depth.h
  psm_ray_probe.h
  psm_sample_backprojection.h
  psm_update_dcdf_implicit.h
  psm_convert_dcdf_implicit.h
)

#if(PSM_USE_OPENGL2_1)
#  set(psm_algo_sources ${psm_algo_sources}
#    psm_render_expected_gpu.h
#  )
#endif(PSM_USE_OPENGL2_1)

aux_source_directory(Templates psm_algo_sources)

add_library(psm_algo ${psm_algo_sources})

target_link_libraries(psm_algo psm vnl vgl vbl bsta hsds vpgl vil vil_algo boct boxm)

#if(PSM_USE_OPENGL2_1)
#  target_link_libraries(psm_algo ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY})
#endif(PSM_USE_OPENGL2_1)


add_subdirectory(tests)
add_subdirectory(pro)






