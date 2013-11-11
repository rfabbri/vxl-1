# contrib/rfabbri/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
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

# Multiview GUI
SET(mw_sources
   mw_menu.cxx       mw_menu.h
   show_contours_process.cxx show_contours_process.h
   mw_app.cxx        mw_app.h
   epip_app.cxx      epip_app.h
   mw_match.cxx      mw_match.h
   mw_curves.cxx      mw_curves.h
   mw_discrete_corresp.cxx      mw_discrete_corresp.h
   mw_rig.cxx        mw_rig.h
   mw_util.cxx       mw_util.h
   geno_app.cxx      geno_app.h
   mw_main.cxx
)

SET(mw_cmd_sources
   mw_app.cxx            mw_app.h
   epip_app.cxx   epip_app.h
   mw_match.cxx      mw_match.h
   mw_curves.cxx      mw_curves.h
   mw_discrete_corresp.cxx      mw_discrete_corresp.h
   mw_rig.cxx       mw_rig.h
   mw_util.cxx      mw_util.h
   mw_cmd.cxx
)


AUX_SOURCE_DIRECTORY(Templates mw_sources)

AUX_SOURCE_DIRECTORY(Templates bvis1_sources)

#ADD_EXECUTABLE(mw ${mw_sources})
ADD_EXECUTABLE(mw ${mw_cmd_sources})

#TARGET_LINK_LIBRARIES(mw dbnl_pro dbdet_vis dbdet_pro dbsol_pro vpgl vidpro1 bpro1 bvis1 dbgui)

TARGET_LINK_LIBRARIES(mw vpgl vidpro1 bpro1 bvis1 dbsol_algo)

ENDIF (VXL_VGUI_FOUND)
