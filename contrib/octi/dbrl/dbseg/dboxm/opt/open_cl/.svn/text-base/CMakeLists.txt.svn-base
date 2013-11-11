# contrib/dbrl/dbseg/dboxm open_cl acceleration library

INCLUDE( ${DBRL_MODULE_PATH}/find_open_cl.cmake )
IF(OPENCL_FOUND)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${DBRL_INCLUDE_DIR}/dbbas/dbocl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

  SET(dboxm_opt_opencl_sources
  boxm_ray_trace_manager.h   boxm_ray_trace_manager.txx 
  boxm_opt_opencl_utils.h  boxm_opt_opencl_utils.cxx
  )
  SET(dbomx_opt_opencl_source_files
      octree_library_functions.cl
  )

  AUX_SOURCE_DIRECTORY(Templates dboxm_opt_opencl_sources)	
  ADD_LIBRARY(dboxm_opt_opencl ${dboxm_opt_opencl_sources}${dboxm_opt_opencl_source_files})

  TARGET_LINK_LIBRARIES(dboxm_opt_opencl dboxm_opt dboxm dbocl bprb bgeo vgl vbl vbl_io ${OPENCL_LIBRARIES})

SUBDIRS(tests)
ENDIF(OPENCL_FOUND)
