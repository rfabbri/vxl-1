# contrib/ozge/gui/CMakeLists.txt

IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )


SET(face_gui_sources
   face_gui_menu.cxx       face_gui_menu.h
   face_gui_main.cxx

   dbru_facedb.cxx       dbru_facedb.h
   dbru_facedb_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates face_gui_sources)

ADD_EXECUTABLE(face_gui ${face_gui_sources})

TARGET_LINK_LIBRARIES(face_gui face_pro face_vis dbdet_pro dbbgm_pro dbbgm_vis vidpro1 bvis1 dbsk2d_pro dbskr_pro dbskr_vis dbinfo_pro dbru_algo dbru_pro dbru_vis dbacm_pro dbsol_pro dbdet_pro dbsol_pro)

SUBDIRS(pro)
SUBDIRS(vis)

ENDIF (VXL_VGUI_FOUND)
