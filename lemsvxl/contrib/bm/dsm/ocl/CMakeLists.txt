#depends on OPENCL being found...
include(${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake)
if(OPENCL_FOUND)

add_definitions(-DHAS_OPENCL=1)
include_directories( ${OPENCL_INCLUDE_PATH} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set( dsm_ocl_sources
     dsm_opencl_processor.h       dsm_opencl_processor.cxx
     dsm_opencl_util.h            dsm_opencl_util.cxx )
     
aux_source_directory(Templates dsm_ocl_sources)     

add_library( dsm_ocl ${dsm_ocl_sources} )     
target_link_libraries( dsm_ocl dsm)
     
#processes     
add_subdirectory( pro )

#tests
add_subdirectory( tests )

endif(OPENCL_FOUND)