# contrib/dec/breg3d_batch/CMakeLists.txt

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
  INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )

   SET(breg3d_batch_sources
       batch_breg3d.h batch_breg3d.cxx
       reg_breg3d.cxx
   )
 

  ADD_LIBRARY(breg3d_batch SHARED ${breg3d_batch_sources})

  #library has to have different name depending on debug or release version.

  SET_TARGET_PROPERTIES(breg3d_batch PROPERTIES OUTPUT_NAME breg3d_batch DEBUG_POSTFIX _d SUFFIX .pyd)

  TARGET_LINK_LIBRARIES(breg3d_batch bprb brdb vbl bvxm_pro breg3d breg3d_pro bvxm_io vpgl_io vpgl_pro vil_io vil_pro )

IF (PYTHON_DEBUG_LIBRARIES)
    TARGET_LINK_LIBRARIES( breg3d_batch debug ${PYTHON_DEBUG_LIBRARY})
ENDIF(PYTHON_DEBUG_LIBRARIES )

IF (PYTHON_LIBRARIES)
    TARGET_LINK_LIBRARIES( breg3d_batch optimized ${PYTHON_LIBRARY})
ENDIF(PYTHON_LIBRARIES)


#ENDIF( VXL_PYTHON_FOUND )
