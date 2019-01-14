# contrib/ozge/gui/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/contrib )


set(face_gui_sources
   face_gui_menu.cxx       face_gui_menu.h
   face_gui_main.cxx

   dbru_facedb.cxx       dbru_facedb.h
   dbru_facedb_sptr.h
)

aux_source_directory(Templates face_gui_sources)

add_executable(face_gui ${face_gui_sources})

target_link_libraries(face_gui face_pro face_vis dbdet_pro dbbgm_pro dbbgm_vis vidpro1 bvis1 dbsk2d_pro dbskr_pro dbskr_vis dbinfo_pro dbru_algo dbru_pro dbru_vis dbacm_pro dbsol_pro dbdet_pro dbsol_pro)

add_subdirectory(pro)
add_subdirectory(vis)

endif (VXL_VGUI_FOUND)
