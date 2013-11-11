IF (VXL_VGUI_FOUND)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
ENDIF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/rfabbri )

# Personal Multiview Lib

SET( mw_sources
   mw_util.txx       mw_util_defs.cxx mw_util.h
   mw_curves.cxx      mw_curves.h
   mw_subpixel_point_set.cxx mw_subpixel_point_set.h
   mw_epi_interceptor.cxx mw_epi_interceptor.h
   mw_intersection_sets.cxx mw_intersection_sets.h
   mw_discrete_corresp.cxx      mw_discrete_corresp.h
   mw_discrete_corresp_3.cxx      mw_discrete_corresp_3.h
   mw_discrete_corresp_n.cxx      mw_discrete_corresp_n.h
   mw_dist.cxx       mw_dist.h
   dbmcs_view_set.cxx dbmcs_view_set.h dbmcs_stereo_views_sptr.h 
   dbmcs_curve_3d_attributes.cxx dbmcs_curve_3d_attributes.h 
   dbmcs_curve_3d_sketch.cxx dbmcs_curve_3d_sketch.h 
)

AUX_SOURCE_DIRECTORY(Templates mw_sources)
AUX_SOURCE_DIRECTORY(Templates bvis1_sources)

ADD_LIBRARY(mw ${mw_sources})

TARGET_LINK_LIBRARIES(mw dborl vpgl dbsol_algo dbnl vil dbul vbl_io)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS(algo)

SUBDIRS(pro)

SUBDIRS(cmd)

IF (VXL_VGUI_FOUND)
  SUBDIRS(vis)
  SUBDIRS(app)
ENDIF (VXL_VGUI_FOUND)

OPTION(BUILD_MATLAB_MEX "Build Matlab mex files" NO)

IF(BUILD_MATLAB_MEX)
  SUBDIRS(mex)
ENDIF(BUILD_MATLAB_MEX)
