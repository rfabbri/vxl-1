# contrib/dbrl/dbbas/docl
include( ${DBRL_MODULE_PATH}/find_open_cl.cmake )
if(OPENCL_FOUND)
  include_directories( ${OPENCL_INCLUDE_PATH} )

  set(dbocl_sources
   opencl_manager.h   opencl_manager.txx 
   bcl_cl.h
  )

  aux_source_directory(Templates dbocl_sources)	
  add_library(dbocl ${dbocl_sources})

  target_link_libraries(dbocl vgl vbl vbl_io ${OPENCL_LIBRARIES})

endif(OPENCL_FOUND)
