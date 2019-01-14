# This is lemsvxlsrc/contrib/isabel/bseg3d/algo/pro/CMakeLists.txt
# bseg3d_algo's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
if(VXL_EXPAT_FOUND)
include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${LEMS_SOURCE_DIR}/contrib/isabel )


set(bseg3d_algo_pro_sources
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

#aux_source_directory(processes bseg3d_algo_pro_sources)

add_library(bseg3d_algo_pro ${bseg3d_algo_pro_sources})

target_link_libraries(bseg3d_algo_pro bseg3d bseg3d_algo bprb vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vil vil_io vil_pro vnl vnl_io bgeo vgl brdb)

#if( BUILD_TESTING )
#  add_subdirectory(tests)
#endif( BUILD_TESTING )
endif(VXL_EXPAT_FOUND)
