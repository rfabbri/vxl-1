# This is contrib/dec/psm/CMakeLists.txt
# Probabalistic Scene Modeling

# Some operations can be optimized using OpenGL 2.1 calls.
#FIND_PACKAGE(OPENGL)
#FIND_PACKAGE(GLUT)
#INCLUDE( ${LEMS_CMAKE_DIR}/Modules/FindGLEW.cmake )
#IF(OPENGL_FOUND)
#  IF (GLUT_FOUND)
#    IF (GLEW_FOUND)
#       OPTION(PSM_USE_OPENGL2_1 "Use OpenGL 2.1 optimizations" NO)
#    ENDIF(GLEW_FOUND)
#  ENDIF(GLUT_FOUND)
#ENDIF(OPENGL_FOUND)

#IF(PSM_USE_OPENGL2_1)
#  ADD_DEFINITIONS( -DPSM_USE_GPU )
#ENDIF(PSM_USE_OPENGL2_1)

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
  
SET(psm_sources
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

AUX_SOURCE_DIRECTORY(Templates psm_sources)


ADD_LIBRARY(psm ${psm_sources})

TARGET_LINK_LIBRARIES(psm vnl vgl vil vil_algo vbl bsta bsta_algo vul hsds bgeo)

SUBDIRS(tests)
SUBDIRS(algo)
SUBDIRS(io)
SUBDIRS(pro)





