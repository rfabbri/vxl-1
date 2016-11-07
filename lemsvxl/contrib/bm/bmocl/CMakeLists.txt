# basic open_cl library

include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)
  add_definitions(-DHAS_OPENCL=1)
  include_directories( ${OPENCL_INCLUDE_PATH} )
  include_directories( ${bm_SOURCE_DIR} )

  set(bmocl_sources
      bmocl_manager.h    bmocl_manager.cxx
      bmocl_manager_sptr.h
      smart_ptr+bmocl_manager-.cxx
      bmocl_cl.h
      bmocl_cl_gl.h
      bmocl_utils.h      bmocl_utils.cxx
  )

  add_library(bmocl ${bmocl_sources} )

  target_link_libraries(bmocl bocl vnl vnl_algo vbl_io vpgl vgl vil vbl vul ${OPENCL_LIBRARIES})

endif(OPENCL_FOUND)