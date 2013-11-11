# basic open_cl library

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)
  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${bm_SOURCE_DIR} )

  SET(bmocl_sources
      bmocl_manager.h    bmocl_manager.cxx
      bmocl_manager_sptr.h
      smart_ptr+bmocl_manager-.cxx
      bmocl_cl.h
      bmocl_cl_gl.h
      bmocl_utils.h      bmocl_utils.cxx
  )

  ADD_LIBRARY(bmocl ${bmocl_sources} )

  TARGET_LINK_LIBRARIES(bmocl bocl vnl vnl_algo vbl_io vpgl vgl vil vbl vul ${OPENCL_LIBRARIES})

ENDIF(OPENCL_FOUND)