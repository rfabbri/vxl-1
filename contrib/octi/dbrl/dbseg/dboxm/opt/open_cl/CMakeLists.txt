# contrib/dbrl/dbseg/dboxm open_cl acceleration library

include( ${DBRL_MODULE_PATH}/find_open_cl.cmake )
if(OPENCL_FOUND)
  include_directories( ${OPENCL_INCLUDE_PATH} )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  include_directories( ${DBRL_INCLUDE_DIR}/dbbas/dbocl )
  include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )

  set(dboxm_opt_opencl_sources
  boxm_ray_trace_manager.h   boxm_ray_trace_manager.txx 
  boxm_opt_opencl_utils.h  boxm_opt_opencl_utils.cxx
  )
  set(dbomx_opt_opencl_source_files
      octree_library_functions.cl
  )

  aux_source_directory(Templates dboxm_opt_opencl_sources)	
  add_library(dboxm_opt_opencl ${dboxm_opt_opencl_sources}${dboxm_opt_opencl_source_files})

  target_link_libraries(dboxm_opt_opencl dboxm_opt dboxm dbocl bprb bgeo vgl vbl vbl_io ${OPENCL_LIBRARIES})

add_subdirectory(tests)
endif(OPENCL_FOUND)
