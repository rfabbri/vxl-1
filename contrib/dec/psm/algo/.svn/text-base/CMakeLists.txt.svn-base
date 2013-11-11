# This is contrib/dec/psm/algo/CMakeLists.txt
# Probabalistic Scene Modeling Algorithms

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
  
#IF(PSM_USE_OPENGL2_1)
#  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
#  INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})
#  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR})
#ENDIF(PSM_USE_OPENGL2_1)

SET(psm_algo_sources
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
  psm_cell_visibility_iterator.h psm_cell_visibility_iterator.txx
  psm_block_visibility_iterator.h psm_block_visibility_iterator.txx
  psm_image_pyramid.h
  psm_triangle_scan_iterator_aa.h  psm_triangle_scan_iterator_aa.cxx
  psm_triangle_scan_iterator.h     psm_triangle_scan_iterator.cxx
  psm_triangle_interpolation_iterator_aa.h  psm_triangle_interpolation_iterator_aa.txx
  psm_triangle_interpolation_iterator.h  psm_triangle_interpolation_iterator.txx
  psm_raytrace_function.h
  psm_convert_to_boct.h
  psm_clean_unobserved.h
  psm_compute_expected_depth.h
  psm_ray_probe.h
  psm_sample_backprojection.h
  psm_update_dcdf_implicit.h
  psm_convert_dcdf_implicit.h
)

#IF(PSM_USE_OPENGL2_1)
#  SET(psm_algo_sources ${psm_algo_sources}
#    psm_render_expected_gpu.h
#  )
#ENDIF(PSM_USE_OPENGL2_1)

AUX_SOURCE_DIRECTORY(Templates psm_algo_sources)

ADD_LIBRARY(psm_algo ${psm_algo_sources})

TARGET_LINK_LIBRARIES(psm_algo psm vnl vgl vbl bsta hsds vpgl vil vil_algo boct boxm)

#IF(PSM_USE_OPENGL2_1)
#  TARGET_LINK_LIBRARIES(psm_algo ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY})
#ENDIF(PSM_USE_OPENGL2_1)


SUBDIRS(tests)
SUBDIRS(pro)






