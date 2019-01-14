# contrib/dec/CMakeLists.txt
# Developement for Dan Crispell

#include_directories( ${LEMS_SOURCE_DIR}/breye )

# 3D registration of video
add_subdirectory(breg3d)
add_subdirectory(breg3d_batch)
add_subdirectory(hsds)
add_subdirectory(psm)
add_subdirectory(psm_batch)
add_subdirectory(psm_opt)
#add_subdirectory(rpc_reg)
add_subdirectory(psm_camera_opt)
#add_subdirectory(psm_local_opt)
add_subdirectory(psm_mfa)










