# contrib/dbrl/dbbas/docl
INCLUDE( ${DBRL_MODULE_PATH}/find_open_cl.cmake )
IF(OPENCL_FOUND)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )

  SET(dbocl_sources
   opencl_manager.h   opencl_manager.txx 
   bcl_cl.h
  )

  AUX_SOURCE_DIRECTORY(Templates dbocl_sources)	
  ADD_LIBRARY(dbocl ${dbocl_sources})

  TARGET_LINK_LIBRARIES(dbocl vgl vbl vbl_io ${OPENCL_LIBRARIES})

ENDIF(OPENCL_FOUND)
