# This is contrib/dec/psm_camera_opt/CMakeLists.txt
# camera optimization library using probabilistic scene models


include_directories( ${LEMS_SOURCE_DIR}/contrib/dec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_MUL_INCLUDE_DIR})

set(psm_camera_opt_sources
    psm_camera_opt_camera_optimizer.h         psm_camera_opt_camera_optimizer.hxx
    psm_camera_opt_ekf_state.h   psm_camera_opt_ekf_state.cxx
    psm_camera_opt_homography_generator.h
    psm_camera_opt_lm_direct_homography_generator.h  psm_camera_opt_lm_direct_homography_generator.hxx
    psm_camera_opt_gdbicp_homography_generator.h  psm_camera_opt_gdbicp_homography_generator.hxx
)

aux_source_directory(Templates psm_camera_opt_sources)

add_library(psm_camera_opt ${psm_camera_opt_sources})

target_link_libraries(psm_camera_opt psm psm_algo vpgl vpgl_algo vil vnl bgeo vgl vgl_algo vsol brip vimt dbvrl)

add_subdirectory(pro)
#add_subdirectory(tests)



