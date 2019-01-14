# contrib/rfabbri/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg)
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/basic )

# Multiview GUI
set(mw_sources
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

set(mw_cmd_sources
   mw_app.cxx            mw_app.h
   epip_app.cxx   epip_app.h
   mw_match.cxx      mw_match.h
   mw_curves.cxx      mw_curves.h
   mw_discrete_corresp.cxx      mw_discrete_corresp.h
   mw_rig.cxx       mw_rig.h
   mw_util.cxx      mw_util.h
   mw_cmd.cxx
)


aux_source_directory(Templates mw_sources)

aux_source_directory(Templates bvis1_sources)

#add_executable(mw ${mw_sources})
add_executable(mw ${mw_cmd_sources})

#target_link_libraries(mw dbnl_pro dbdet_vis dbdet_pro dbsol_pro vpgl vidpro1 bpro1 bvis1 dbgui)

target_link_libraries(mw vpgl vidpro1 bpro1 bvis1 dbsol_algo)

endif (VXL_VGUI_FOUND)
