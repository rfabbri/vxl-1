# This is contrib/dec/psm/CMakeLists.txt
# Probabalistic Scene Modeling

# Some operations can be optimized using OpenGL 2.1 calls.
#find_package(OPENGL)
#find_package(GLUT)
#include( ${LEMS_CMAKE_DIR}/Modules/FindGLEW.cmake )
#if(OPENGL_FOUND)
#  if (GLUT_FOUND)
#    if (GLEW_FOUND)
#       option(PSM_USE_OPENGL2_1 "Use OpenGL 2.1 optimizations" NO)
#    endif(GLEW_FOUND)
#  endif(GLUT_FOUND)
#endif(OPENGL_FOUND)

#if(PSM_USE_OPENGL2_1)
#  add_definitions( -DPSM_USE_GPU )
#endif(PSM_USE_OPENGL2_1)

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${lems_SOURCE_DIR}/contrib/dec )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
  
set(psm_sources
  psm_scene_base.h  psm_scene_base.cxx
  psm_scene.h  psm_scene.txx
  psm_sample.h
  psm_cell_id.h
  psm_image_sample.h
  psm_scene_block_storage.h
  psm_scene_block_storage_single.h   psm_scene_block_storage_single.txx  
  psm_scene_block_storage_cached.h   psm_scene_block_storage_cached.txx
  psm_scene_block_storage_null.h
  psm_apm_traits.h
  psm_cell_enumerator.h
  psm_util.h
  psm_mog_grey_processor.h  psm_mog_grey_processor.cxx
  psm_simple_grey_processor.h  psm_simple_grey_processor.cxx
  psm_mog_rgb_processor.h   psm_mog_rgb_processor.cxx
  psm_simple_rgb_processor.h   psm_simple_rgb_processor.cxx
  psm_aux_scene_base.h  psm_aux_scene_base.cxx
  psm_aux_scene.h  psm_aux_scene.txx
  psm_vis_implicit_sample.h
  psm_dcdf_implicit_sample.h
  psm_update_sample.h
  psm_aux_traits.h
  psm_sigma_normalizer.h psm_sigma_normalizer.cxx
)

aux_source_directory(Templates psm_sources)


add_library(psm ${psm_sources})

target_link_libraries(psm vnl vgl vil vil_algo vbl bsta bsta_algo vul hsds bgeo)

add_subdirectory(tests)
add_subdirectory(algo)
add_subdirectory(io)
add_subdirectory(pro)





