# contrib/seg/gui/CMakeLists.txt

IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )


SET(dbseg_gui_sources
   seg_gui_menu.cxx                        seg_gui_menu.h
   seg_gui_main.cxx                    
 
    
)
ADD_EXECUTABLE(dbseg_gui ${dbseg_gui_sources})
TARGET_LINK_LIBRARIES(dbseg_gui dbseg_pro dbseg_vis vidpro1 bvis1 dbacm_pro dbil_pro dbmsh3d_algovtk dbmsh3d_algovtk_pro)

ENDIF (VXL_VGUI_FOUND)


