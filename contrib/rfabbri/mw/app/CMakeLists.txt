IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/rfabbri )


# Personal Multiview Lib

# Multiview GUI
SET(mw_sources
   mw_menu.cxx       mw_menu.h
   show_contours_process.cxx show_contours_process.h
   mw_app.cxx        mw_app.h
   differential-estimates/mw_gradient_descent.cxx differential-estimates/mw_gradient_descent.h
   occluding-contours/ctspheres_app.cxx occluding-contours/ctspheres_app.h
   occluding-contours/ctspheres_io.cxx occluding-contours/ctspheres_io.h
   geno_app.cxx      geno_app.h
   mw_main.cxx
)

SET(mw_stereo_sources
   stereo/mw_stereo_menu.cxx       stereo/mw_stereo_menu.h
   show_contours_process.cxx show_contours_process.h
   mw_app.cxx        mw_app.h
   differential-estimates/mw_gradient_descent.cxx differential-estimates/mw_gradient_descent.h
   occluding-contours/ctspheres_app.cxx occluding-contours/ctspheres_app.h
   occluding-contours/ctspheres_io.cxx occluding-contours/ctspheres_io.h
   mw_load_data.cxx mw_load_data.h
   stereo/mw_stereo_main.cxx
)

SET(mw_stereo_cmd_sources
   stereo/mw_stereo_cmd.cxx
)

SET(mw_stereo_exp_sources
   stereo/mw_stereo_experiments_cmd.cxx
)

SET(mw_cmd_sources
   mw_cmd.cxx
)

SET(mw_perturb_cmd_sources
#   mw_app.cxx            mw_app.h
   occluding-contours/ctspheres_app.cxx occluding-contours/ctspheres_app.h
   occluding-contours/ctspheres_io.cxx occluding-contours/ctspheres_io.h
   occluding-contours/mw_perturb_cmd.cxx
)

SET(mw_k_estimation_cmd_sources
   mw_app.cxx            mw_app.h
   differential-estimates/mw_gradient_descent.cxx differential-estimates/mw_gradient_descent.h
   differential-estimates/mw_k_estimation_cmd.cxx
)

SET(mw_sel_geometry_cmd_sources
   differential-estimates/mw_sel_geometry_cmd.cxx 
)

#ADD_EXECUTABLE(perturb_cmd ${mw_perturb_cmd_sources})
#ADD_EXECUTABLE(estimation ${mw_k_estimation_cmd_sources})
#ADD_EXECUTABLE(cmd ${mw_cmd_sources})
#ADD_EXECUTABLE(gui ${mw_sources})
#ADD_EXECUTABLE(sel_geom ${mw_sel_geometry_cmd_sources})
ADD_EXECUTABLE(sgui ${mw_stereo_sources})
ADD_EXECUTABLE(scmd ${mw_stereo_cmd_sources})
ADD_EXECUTABLE(sexp ${mw_stereo_exp_sources})

#TARGET_LINK_LIBRARIES(perturb_cmd mw bvis1 vil dvpgl_pro dbdet) 
#TARGET_LINK_LIBRARIES(gui mw_pro mw_vis dbnl_pro dbdet_vis dbdet_pro dbsol_pro dbcvr_vis dbcvr_pro dvpgl_pro dvpgl_vis vidpro1 dbctrk_vis dbecl_vis dbetl_vis bpro1 dbgui mvl) 
#TARGET_LINK_LIBRARIES(sel_geom mw_pro mw_vis dbdet_vis dbdet_pro dbsol_pro)
#TARGET_LINK_LIBRARIES(cmd mw_algo) 
#TARGET_LINK_LIBRARIES(estimation mw vpgl dbsol_algo vidpro1 bpro1 bvis1)
TARGET_LINK_LIBRARIES(sgui mw_pro mw_algo mw_vis dbdet_vis dbdet_pro dbkpr_vis dbsol_pro dvpgl_pro dvpgl_vis vpgl_algo dvpgl_algo_pro dbdif_pro vidpro1 dbetl_vis dbil_pro mvl)
TARGET_LINK_LIBRARIES(scmd mw_pro dbdif_pro dbkpr_pro vpgl_algo) 
TARGET_LINK_LIBRARIES(sexp mw_pro testlib) 
# Requires a CMakeLists.txt file if SUBDIRS - JLM
#SUBDIRS(stereo)

ENDIF (VXL_VGUI_FOUND)
