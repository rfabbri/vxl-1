if (VXL_VGUI_FOUND)

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/bmvgd)
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/basic )


include_directories( ${LEMS_SOURCE_DIR}/contrib/rfabbri )


# Personal Multiview Lib

# Multiview GUI
set(mw_sources
   mw_menu.cxx       mw_menu.h
   show_contours_process.cxx show_contours_process.h
   mw_app.cxx        mw_app.h
   differential-estimates/mw_gradient_descent.cxx differential-estimates/mw_gradient_descent.h
   occluding-contours/ctspheres_app.cxx occluding-contours/ctspheres_app.h
   occluding-contours/ctspheres_io.cxx occluding-contours/ctspheres_io.h
   geno_app.cxx      geno_app.h
   mw_main.cxx
)

set(mw_stereo_sources
   stereo/mw_stereo_menu.cxx       stereo/mw_stereo_menu.h
   show_contours_process.cxx show_contours_process.h
   mw_app.cxx        mw_app.h
   differential-estimates/mw_gradient_descent.cxx differential-estimates/mw_gradient_descent.h
   occluding-contours/ctspheres_app.cxx occluding-contours/ctspheres_app.h
   occluding-contours/ctspheres_io.cxx occluding-contours/ctspheres_io.h
   mw_load_data.cxx mw_load_data.h
   stereo/mw_stereo_main.cxx
)

set(mw_stereo_cmd_sources
   stereo/mw_stereo_cmd.cxx
)

set(mw_stereo_exp_sources
   stereo/mw_stereo_experiments_cmd.cxx
)

set(mw_cmd_sources
   mw_cmd.cxx
)

set(mw_perturb_cmd_sources
#   mw_app.cxx            mw_app.h
   occluding-contours/ctspheres_app.cxx occluding-contours/ctspheres_app.h
   occluding-contours/ctspheres_io.cxx occluding-contours/ctspheres_io.h
   occluding-contours/mw_perturb_cmd.cxx
)

set(mw_k_estimation_cmd_sources
   mw_app.cxx            mw_app.h
   differential-estimates/mw_gradient_descent.cxx differential-estimates/mw_gradient_descent.h
   differential-estimates/mw_k_estimation_cmd.cxx
)

set(mw_sel_geometry_cmd_sources
   differential-estimates/mw_sel_geometry_cmd.cxx 
)

#add_executable(perturb_cmd ${mw_perturb_cmd_sources})
#add_executable(estimation ${mw_k_estimation_cmd_sources})
#add_executable(cmd ${mw_cmd_sources})
#add_executable(gui ${mw_sources})
#add_executable(sel_geom ${mw_sel_geometry_cmd_sources})
add_executable(sgui ${mw_stereo_sources})
add_executable(scmd ${mw_stereo_cmd_sources})
add_executable(sexp ${mw_stereo_exp_sources})

#target_link_libraries(perturb_cmd mw bvis1 vil vpgld_pro dbdet) 
#target_link_libraries(gui mw_pro mw_vis bnld_pro dbdet_vis dbdet_pro bsold_pro dbcvr_vis dbcvr_pro vpgld_pro vpgld_vis vidpro1 dbctrk_vis becld_vis dbetl_vis bpro1 dbgui mvl) 
#target_link_libraries(sel_geom mw_pro mw_vis dbdet_vis dbdet_pro bsold_pro)
#target_link_libraries(cmd mw_algo) 
#target_link_libraries(estimation mw vpgl bsold_algo vidpro1 bpro1 bvis1)
target_link_libraries(sgui mw_pro mw_algo mw_vis dbdet_vis dbdet_pro dbkpr_vis dbkpr_pro bmcsd_pro bsold_pro vpgld_pro vpgld_vis vpgl_algo vpgld_algo_pro bdifd_pro bgld vidpro1 dbetl_vis bild_pro mvl)
target_link_libraries(scmd mw_pro bdifd_pro dbkpr_pro vpgl_algo) 
target_link_libraries(sexp mw_pro testlib) 
# Requires a CMakeLists.txt file if SUBDIRS - JLM
#add_subdirectory(stereo)

endif (VXL_VGUI_FOUND)
