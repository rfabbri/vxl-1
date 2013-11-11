# basic OpenCL Vector Add example

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)
  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
  INCLUDE_DIRECTORIES( ${bm_SOURCE_DIR}/bmocl )

  SET(oclVectorAdd_sources
      oclVectorAdd.cxx
      oclVectorAddManager.h    oclVectorAddManager.cxx
  )

  SET(oclVectorAdd_cl_sources
      cl/oclVectorAdd.cl )
      
  FOREACH( KERNEL ${oclVectorAdd_cl_sources} )      
    #MESSAGE( "KERNEL = " ${KERNEL})
    #MESSAGE( "CMAKE_SOURCE_DIR = " ${CMAKE_CURRENT_SOURCE_DIR} )
    #MESSAGE( "CMAKE_BINARY_DIR = " ${CMAKE_CURRENT_BINARY_DIR} )
    CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/${KERNEL} ${CMAKE_CURRENT_BINARY_DIR}/${KERNEL})
  ENDFOREACH(KERNEL ${oclVectorAdd_cl_sources} )
  
  ADD_EXECUTABLE(oclVectorAdd ${oclVectorAdd_sources} ${oclVectorAdd_cl_sources} )
  
  TARGET_LINK_LIBRARIES( oclVectorAdd bmocl vbl ${OPENCL_LIBRARIES})

ENDIF(OPENCL_FOUND)