# This is contrib/dec/psm_camera_opt/CMakeLists.txt
# camera optimization library using probabilistic scene models


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})

SET(psm_camera_opt_sources
    psm_camera_opt_camera_optimizer.h         psm_camera_opt_camera_optimizer.txx
    psm_camera_opt_ekf_state.h   psm_camera_opt_ekf_state.cxx
    psm_camera_opt_homography_generator.h
    psm_camera_opt_lm_direct_homography_generator.h  psm_camera_opt_lm_direct_homography_generator.txx
    psm_camera_opt_gdbicp_homography_generator.h  psm_camera_opt_gdbicp_homography_generator.txx
)

AUX_SOURCE_DIRECTORY(Templates psm_camera_opt_sources)

ADD_LIBRARY(psm_camera_opt ${psm_camera_opt_sources})

TARGET_LINK_LIBRARIES(psm_camera_opt psm psm_algo vpgl vpgl_algo vil vnl bgeo vgl vgl_algo vsol brip vimt dbvrl)

SUBDIRS(pro)
#SUBDIRS(tests)



