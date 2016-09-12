# contrib/dec/breg3d_batch/CMakeLists.txt

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
  include_directories( ${VXL_MUL_INCLUDE_DIR} )

  include_directories( ${lems_SOURCE_DIR}/contrib/dec )

   set(breg3d_batch_sources
       batch_breg3d.h batch_breg3d.cxx
       reg_breg3d.cxx
   )
 

  add_library(breg3d_batch SHARED ${breg3d_batch_sources})

  #library has to have different name depending on debug or release version.

  set_target_properties(breg3d_batch PROPERTIES OUTPUT_NAME breg3d_batch DEBUG_POSTFIX _d SUFFIX .pyd)

  target_link_libraries(breg3d_batch bprb brdb vbl bvxm_pro breg3d breg3d_pro bvxm_io vpgl_io vpgl_pro vil_io vil_pro )

if (PYTHON_DEBUG_LIBRARIES)
    target_link_libraries( breg3d_batch debug ${PYTHON_DEBUG_LIBRARY})
endif(PYTHON_DEBUG_LIBRARIES )

if (PYTHON_LIBRARIES)
    target_link_libraries( breg3d_batch optimized ${PYTHON_LIBRARY})
endif(PYTHON_LIBRARIES)


#endif( VXL_PYTHON_FOUND )
