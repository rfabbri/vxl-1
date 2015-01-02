# brl/bmvl/brct/CMakeLists.txt

# 3D reconstruction from multiple views library

INCLUDE( ${LEMS_CMAKE_DIR}/config/vxl_utils.cmake )

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/mvg/bmvl )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_RPL_INCLUDE_DIR} )

SET(brct_sources
    brct_algos.h                  brct_algos.cpp
    kalman_filter.h               kalman_filter.cpp
    brct_structure_estimator.h    brct_structure_estimator.cxx
    brct_epi_reconstructor.h      brct_epi_reconstructor.cxx
    brct_plane_calibrator.h       brct_plane_calibrator.cxx
    brct_corr.h                   brct_corr.cxx          brct_corr_sptr.h
    brct_plane_corr.h             brct_plane_corr.cxx    brct_plane_corr_sptr.h
    brct_plane_sweeper_params.h   brct_plane_sweeper_params.cxx
    brct_plane_sweeper.h          brct_plane_sweeper.cxx
    brct_volume_processor_params.h    brct_volume_processor_params.cxx
    brct_volume_processor.h       brct_volume_processor.cxx
)
AUX_SOURCE_DIRECTORY(Templates brct_sources)


SET(brct_sources ${brct_sources}
    brct_dense_reconstructor.h    brct_dense_reconstructor.cxx
)


ADD_LIBRARY(brct ${brct_sources})
INSTALL_TARGETS(/lib brct)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bmvl/brct ${brct_sources})

TARGET_LINK_LIBRARIES(brct sdet brip rrel bsol vpgl gevd vdgl vsol mvl bdgl bugl vil vil1 vgl_algo vgl vnl_algo vnl vbl)
TARGET_LINK_LIBRARIES(brct vsrl)


IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
