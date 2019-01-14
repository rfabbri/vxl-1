include_directories( ${VXL_PYTHON_INCLUDE_PATH} )
if (WIN32)
  include_directories( ${VXL_PYTHON_PC_INCLUDE_PATH} )
endif(WIN32)
  include_directories( ${VXL_BRL_INCLUDE_DIR} )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
  include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/core )

  include_directories( ${VXL_CORE_INCLUDE_DIR} )
  include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
  include_directories( ${VXL_GEL_INCLUDE_DIR} )
  include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
  include_directories(${VXL_MUL_INCLUDE_DIR} )
  include_directories( ${LEMS_SOURCE_DIR}/contrib )

  set(dbvxm_batch_sources      
          batch_dbvxm.h batch_dbvxm.cxx
      reg_dbvxm.cxx
   )
 

  add_library(dbvxm_batch SHARED ${dbvxm_batch_sources})

  #library has to have different name depending on debug or release version.

  set_target_properties(dbvxm_batch PROPERTIES OUTPUT_NAME dbvxm_batch DEBUG_POSTFIX _d SUFFIX .pyd)

  target_link_libraries(dbvxm_batch dbvxm_pro dbvxm_rec_pro bprb brdb vbl bvxm bvxm_pro bvxm_io vpgl_io vpgl_pro bvgl_pro vil_io vil_pro debug ${VXL_PYTHON_DEBUG_LIBRARY} optimized ${VXL_PYTHON_LIBRARY})

  if (PYTHON_DEBUG_LIBRARIES)
    target_link_libraries( dbvxm_batch debug ${PYTHON_DEBUG_LIBRARY})
  endif(PYTHON_DEBUG_LIBRARIES )

  if(PYTHON_LIBRARIES)
    target_link_libraries( dbvxm_batch optimized ${PYTHON_LIBRARY})
  endif(PYTHON_LIBRARIES)

#old cmakelist file
# dbvxm_batch/CMakeLists.txt
#include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
#if( PYTHON_FOUND )
#  #include( ${VXL_BRL_INCLUDE_DIR}/bmods/FindEXPAT.cmake )
#  #if( EXPAT_FOUND )
#    include_directories( ${PYTHON_INCLUDE_PATH} )
#    include_directories( ${VXL_BRL_INCLUDE_DIR} )
#    include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
#    include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro/core )
#
#    include_directories( ${VXL_CORE_INCLUDE_DIR} )
#    include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
#    include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
#    include_directories( ${VXL_MUL_INCLUDE_DIR} )
#
#    include_directories( ${VXL_GEL_INCLUDE_DIR})
#
#    include_directories( ${LEMS_SOURCE_DIR}/contrib/ )
#    include_directories( ${LEMS_SOURCE_DIR}/contrib/dbvxm )
#
#    set(dbvxm_batch_sources
#      batch_dbvxm.h batch_dbvxm.cxx
#      reg_dbvxm.cxx
#    )
# 
#
#    add_library(dbvxm_batch SHARED ${dbvxm_batch_sources})
#
#    #library has to have different name depending on debug or release version.
#
#    set_target_properties(dbvxm_batch PROPERTIES OUTPUT_NAME dbvxm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
#
#    target_link_libraries(dbvxm_batch bprb brdb vbl bvxm_pro dbvxm_pro bvxm_io vpgl_io vpgl_pro vil_io vil_pro debug ${PYTHON_DEBUG_LIBRARY} optimized ${PYTHON_LIBRARY})
#  
#    if (PYTHON_DEBUG_LIBRARIES)
#      target_link_libraries( dbvxm_batch debug ${PYTHON_DEBUG_LIBRARY})
#    endif(PYTHON_DEBUG_LIBRARIES )
#
#    if(PYTHON_LIBRARIES)
#      target_link_libraries( dbvxm_batch optimized ${PYTHON_LIBRARY})
#    endif(PYTHON_LIBRARIES)
#
#  #endif( EXPAT_FOUND )
#endif( PYTHON_FOUND )
