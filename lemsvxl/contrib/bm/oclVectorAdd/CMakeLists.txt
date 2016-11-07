# basic OpenCL Vector Add example

include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)
  add_definitions(-DHAS_OPENCL=1)
  include_directories( ${OPENCL_INCLUDE_PATH} )
  include_directories( ${BRL_INCLUDE_DIR}/bbas )
  include_directories( ${bm_SOURCE_DIR}/bmocl )

  set(oclVectorAdd_sources
      oclVectorAdd.cxx
      oclVectorAddManager.h    oclVectorAddManager.cxx
  )

  set(oclVectorAdd_cl_sources
      cl/oclVectorAdd.cl )
      
  foreach( KERNEL ${oclVectorAdd_cl_sources} )      
    #message( "KERNEL = " ${KERNEL})
    #message( "CMAKE_SOURCE_DIR = " ${CMAKE_CURRENT_SOURCE_DIR} )
    #message( "CMAKE_BINARY_DIR = " ${CMAKE_CURRENT_BINARY_DIR} )
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/${KERNEL} ${CMAKE_CURRENT_BINARY_DIR}/${KERNEL})
  endforeach(KERNEL ${oclVectorAdd_cl_sources} )
  
  add_executable(oclVectorAdd ${oclVectorAdd_sources} ${oclVectorAdd_cl_sources} )
  
  target_link_libraries( oclVectorAdd bmocl vbl ${OPENCL_LIBRARIES})

endif(OPENCL_FOUND)