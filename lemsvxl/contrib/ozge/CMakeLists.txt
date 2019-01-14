# 3D Model Registration Applications
# contrib/ozge/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/basic )

project( ozge )

# dll library for recognition of video objects in each frame
#add_subdirectory( rec_exe )

# my simple processes
add_subdirectory( pro )

# my command line executables
add_subdirectory( bin )
add_subdirectory( dbskr_bin )

# my simple video player, modified vidfpl_vil from vxl/contrib/brl/vvid
#add_subdirectory( vidl_gui )
#add_subdirectory( face_gui )

#include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

# fit lines to input curve 
#set(cv_fitter
#   curvefitting_command.cxx
#)

# random curve sampler
#set(cv_sampler
#   random_curve_sampler.cxx
#)

#add_executable(cv_sampler_exec ${cv_sampler})

# random subsampler, subsamples a given .con contour
#set(cv_subsampler
#   random_subsampler.cxx
#)

#add_executable(cv_fitter_exec ${cv_fitter})
#add_executable(cv_sampler_exec ${cv_sampler})
#add_executable(cv_subsampler_exec ${cv_subsampler})

#target_link_libraries( cv_subsampler_exec bsol dbcvr dbsol dbsol_algo )
#target_link_libraries( cv_fitter_exec bsol vgl )

#if(HAS_BGUI3D)

#include_directories( ${LEMS_SOURCE_DIR}/basic )
#include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )

# model registration sources
#set(mreg_sources
#   iv2ras.cxx
#)

#set(mreg_sources2
#   iv2rgb.cxx
#)

#set(mreg_sources3
#   iv2pov.cxx
#)


#add_executable(mreg_app ${mreg_sources})
#add_executable(mreg_app2 ${mreg_sources2})
#add_executable(mreg_app3 ${mreg_sources3})

#target_link_libraries( mreg_app bgui3d )
#target_link_libraries( mreg_app2 bgui3d )
#target_link_libraries( mreg_app3 bgui3d )


#endif(HAS_BGUI3D)
