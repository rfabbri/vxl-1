# contrib/bvam_batch/CMakeLists.txt

#update lemsvxl/config/Modules folder to get the following
#include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
#if( VXL_PYTHON_FOUND )
  include_directories( ${VXL_PYTHON_INCLUDE_PATH} )
if (WIN32)
  include_directories( ${VXL_PYTHON_PC_INCLUDE_PATH} )
endif(WIN32)
  include_directories( ${VXL_BRL_INCLUDE_DIR} )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/core )

  include_directories( ${VXL_CORE_INCLUDE_DIR} )
  include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
  include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
  include_directories(${VXL_MUL_INCLUDE_DIR} )
  include_directories( ${lems_SOURCE_DIR}/contrib )

   set(bvam_batch_sources
       batch_bvam.h batch_bvam.cxx
       reg_bvam.cxx
   )
 

  add_library(bvam_batch SHARED ${bvam_batch_sources})

  #library has to have different name depending on debug or release version.

  set_target_properties(bvam_batch PROPERTIES OUTPUT_NAME bvam_batch DEBUG_POSTFIX _d SUFFIX .pyd)

  target_link_libraries(bvam_batch bprb brdb vbl bvam_pro bvam_algo bvam_algo_pro bvam_io vpgl_io vpgl_bpro vil_io vil_bpro debug ${VXL_PYTHON_DEBUG_LIBRARY} optimized ${VXL_PYTHON_LIBRARY})

#endif( VXL_PYTHON_FOUND )
