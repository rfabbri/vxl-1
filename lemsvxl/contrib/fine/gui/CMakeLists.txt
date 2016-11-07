# contrib/seg/gui/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )


set(dbseg_gui_sources
   seg_gui_menu.cxx                        seg_gui_menu.h
   seg_gui_main.cxx                    
 
    
)
add_executable(dbseg_gui ${dbseg_gui_sources})
target_link_libraries(dbseg_gui dbseg_pro dbseg_vis vidpro1 bvis1 dbacm_pro dbil_pro dbmsh3d_algovtk dbmsh3d_algovtk_pro)

endif (VXL_VGUI_FOUND)


