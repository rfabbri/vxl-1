#depends on OPENCL being found...
INCLUDE(${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake)
IF(OPENCL_FOUND)

ADD_DEFINITIONS(-DHAS_OPENCL=1)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET( dsm_ocl_sources
     dsm_opencl_processor.h       dsm_opencl_processor.cxx
     dsm_opencl_util.h            dsm_opencl_util.cxx )
     
AUX_SOURCE_DIRECTORY(Templates dsm_ocl_sources)     

ADD_LIBRARY( dsm_ocl ${dsm_ocl_sources} )     
TARGET_LINK_LIBRARIES( dsm_ocl dsm)
     
#processes     
SUBDIRS( pro )

#tests
SUBDIRS( tests )

ENDIF(OPENCL_FOUND)