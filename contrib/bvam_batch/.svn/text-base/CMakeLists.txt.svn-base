# contrib/bvam_batch/CMakeLists.txt

#update lemsvxl/config/Modules folder to get the following
#INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
#IF( VXL_PYTHON_FOUND )
  INCLUDE_DIRECTORIES( ${VXL_PYTHON_INCLUDE_PATH} )
IF (WIN32)
  INCLUDE_DIRECTORIES( ${VXL_PYTHON_PC_INCLUDE_PATH} )
ENDIF(WIN32)
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro/core )

  INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

   SET(bvam_batch_sources
       batch_bvam.h batch_bvam.cxx
       reg_bvam.cxx
   )
 

  ADD_LIBRARY(bvam_batch SHARED ${bvam_batch_sources})

  #library has to have different name depending on debug or release version.

  SET_TARGET_PROPERTIES(bvam_batch PROPERTIES OUTPUT_NAME bvam_batch DEBUG_POSTFIX _d SUFFIX .pyd)

  TARGET_LINK_LIBRARIES(bvam_batch bprb brdb vbl bvam_pro bvam_algo bvam_algo_pro bvam_io vpgl_io vpgl_bpro vil_io vil_bpro debug ${VXL_PYTHON_DEBUG_LIBRARY} optimized ${VXL_PYTHON_LIBRARY})

#ENDIF( VXL_PYTHON_FOUND )
