# This is lemsvxlsrc/contrib/isabel/bseg3d/algo/pro/CMakeLists.txt
# bseg3d_algo's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
IF(VXL_EXPAT_FOUND)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/isabel )


SET(bseg3d_algo_pro_sources
    bseg3d_algo_processes.h
    bseg3d_algo_register.h  bseg3d_algo_register.cxx   
    processes/bseg3d_explore_mixtures_process.cxx
    processes/bseg3d_threshold_world_process.cxx
    processes/bseg3d_l2distance_process.cxx
    processes/bseg3d_save_float_grid_process.cxx
    processes/bseg3d_merge_mixtures_process.cxx
    processes/bseg3d_multiply_grids_process.cxx
    processes/bseg3d_convolve_window_process.cxx
    processes/bseg3d_crop_grid_process.cxx
)

#AUX_SOURCE_DIRECTORY(processes bseg3d_algo_pro_sources)

ADD_LIBRARY(bseg3d_algo_pro ${bseg3d_algo_pro_sources})

TARGET_LINK_LIBRARIES(bseg3d_algo_pro bseg3d bseg3d_algo bprb vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vil vil_io vil_pro vnl vnl_io bgeo vgl brdb)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )
ENDIF(VXL_EXPAT_FOUND)
