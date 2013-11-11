# contrib/mleotta/modrec/CMakeLists.txt
# 3D model based recognition Library.

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

SET(modrec_sources
  modrec_proj_cost_func.cxx         modrec_proj_cost_func.h
  modrec_proj_lsqr_cost_func.cxx    modrec_proj_lsqr_cost_func.h

  modrec_vehicle_mesh.cxx           modrec_vehicle_mesh.h
  modrec_vehicle_parts.cxx          modrec_vehicle_parts.h
  modrec_vehicle_fit.cxx            modrec_vehicle_fit.h
  modrec_vehicle_fit_multiview.cxx  modrec_vehicle_fit_multiview.h
  modrec_vehicle_fit_video.cxx      modrec_vehicle_fit_video.h
  modrec_vehicle_state.cxx          modrec_vehicle_state.h
  modrec_vehicle_tracker.cxx        modrec_vehicle_tracker.h
  modrec_vehicle_track_init.cxx     modrec_vehicle_track_init.h
  modrec_pca_vehicle.cxx            modrec_pca_vehicle.h
  modrec_pca_vehicle_projector.cxx  modrec_pca_vehicle_projector.h

  modrec_edgel.cxx                  modrec_edgel.h

  modrec_feature_3d.h
  modrec_codeword.h
)

#AUX_SOURCE_DIRECTORY(Templates modrec_sources)

ADD_LIBRARY(modrec  ${modrec_sources})

TARGET_LINK_LIBRARIES(modrec dbpro dbvidl2_pro dbil_pro bbgm imesh_algo vpgl vil_algo rrel vgel)

IF( BUILD_TESTING )
#  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

# Make the Python Module for running experiemnts
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
  
  SET(py_modrec_sources
      py_modrec.cxx
      py_modrec_manager.cxx   py_modrec_manager.h
  )

  ADD_LIBRARY(py_modrec MODULE ${py_modrec_sources})

  IF(WIN32)
    #library has to have different name depending on debug or release version.
    SET_TARGET_PROPERTIES(py_modrec PROPERTIES OUTPUT_NAME py_modrec DEBUG_POSTFIX _d SUFFIX .pyd)
  ENDIF(WIN32)
  SET_TARGET_PROPERTIES(py_modrec PROPERTIES PREFIX "")

  TARGET_LINK_LIBRARIES(py_modrec modrec dbul) 

  IF (PYTHON_DEBUG_LIBRARIES)
    TARGET_LINK_LIBRARIES( py_modrec debug ${PYTHON_DEBUG_LIBRARY})
  ENDIF(PYTHON_DEBUG_LIBRARIES )

  IF(PYTHON_LIBRARIES)
    TARGET_LINK_LIBRARIES( py_modrec optimized ${PYTHON_LIBRARY})
  ENDIF(PYTHON_LIBRARIES)
ENDIF( PYTHON_FOUND )


# binary IO 
SUBDIRS( io ) 

# processes
SUBDIRS( pro )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
