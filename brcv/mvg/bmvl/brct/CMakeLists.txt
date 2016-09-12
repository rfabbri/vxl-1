# brl/bmvl/brct/CMakeLists.txt

# 3D reconstruction from multiple views library

include( ${LEMS_CMAKE_DIR}/config/vxl_utils.cmake )

include_directories(${VXL_BRL_INCLUDE_DIR})
include_directories(${lems_SOURCE_DIR}/brcv/mvg/bmvl )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_RPL_INCLUDE_DIR} )

set(brct_sources
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
aux_source_directory(Templates brct_sources)


set(brct_sources ${brct_sources}
    brct_dense_reconstructor.h    brct_dense_reconstructor.cxx
)


add_library(brct ${brct_sources})
install_targets(/lib brct)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bmvl/brct ${brct_sources})

target_link_libraries(brct sdet brip rrel bsol vpgl gevd vdgl vsol mvl bdgl bugl vil vil1 vgl_algo vgl vnl_algo vnl vbl)
target_link_libraries(brct vsrl)


if(BUILD_TESTING)
  subdirs(tests)
endif(BUILD_TESTING)
