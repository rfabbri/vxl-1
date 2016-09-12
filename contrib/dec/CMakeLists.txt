# contrib/dec/CMakeLists.txt
# Developement for Dan Crispell

#include_directories( ${lems_SOURCE_DIR}/breye )

# 3D registration of video
subdirs(breg3d)
subdirs(breg3d_batch)
subdirs(hsds)
subdirs(psm)
subdirs(psm_batch)
subdirs(psm_opt)
#subdirs(rpc_reg)
subdirs(psm_camera_opt)
#subdirs(psm_local_opt)
subdirs(psm_mfa)










