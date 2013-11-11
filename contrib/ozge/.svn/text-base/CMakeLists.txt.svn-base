# 3D Model Registration Applications
# contrib/ozge/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

PROJECT( ozge )

# dll library for recognition of video objects in each frame
#SUBDIRS ( rec_exe )

# my simple processes
#SUBDIRS( pro )

# my command line executables
SUBDIRS( bin )
SUBDIRS( dbskr_bin )

# my simple video player, modified vidfpl_vil from vxl/contrib/brl/vvid
#SUBDIRS( vidl_gui )
#SUBDIRS( face_gui )

#INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

# fit lines to input curve 
#SET(cv_fitter
#   curvefitting_command.cxx
#)

# random curve sampler
#SET(cv_sampler
#   random_curve_sampler.cxx
#)

#ADD_EXECUTABLE(cv_sampler_exec ${cv_sampler})

# random subsampler, subsamples a given .con contour
#SET(cv_subsampler
#   random_subsampler.cxx
#)

#ADD_EXECUTABLE(cv_fitter_exec ${cv_fitter})
#ADD_EXECUTABLE(cv_sampler_exec ${cv_sampler})
#ADD_EXECUTABLE(cv_subsampler_exec ${cv_subsampler})

#TARGET_LINK_LIBRARIES( cv_subsampler_exec bsol dbcvr dbsol dbsol_algo )
#TARGET_LINK_LIBRARIES( cv_fitter_exec bsol vgl )

#IF(HAS_BGUI3D)

#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

# model registration sources
#SET(mreg_sources
#   iv2ras.cxx
#)

#SET(mreg_sources2
#   iv2rgb.cxx
#)

#SET(mreg_sources3
#   iv2pov.cxx
#)


#ADD_EXECUTABLE(mreg_app ${mreg_sources})
#ADD_EXECUTABLE(mreg_app2 ${mreg_sources2})
#ADD_EXECUTABLE(mreg_app3 ${mreg_sources3})

#TARGET_LINK_LIBRARIES( mreg_app bgui3d )
#TARGET_LINK_LIBRARIES( mreg_app2 bgui3d )
#TARGET_LINK_LIBRARIES( mreg_app3 bgui3d )


#ENDIF(HAS_BGUI3D)
