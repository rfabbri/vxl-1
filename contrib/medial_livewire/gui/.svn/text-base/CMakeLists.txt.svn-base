# contrib/medial_livewire/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg/dbdet )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib)


SET(medial_livewire_gui_sources
   medial_livewire_menu.cxx       medial_livewire_menu.h
   medial_livewire_main.cxx
   dbdet_draw_spheres_tool.cxx    dbdet_draw_spheres_tool.h
   dbdet_dlvwr_params.cxx         dbdet_dlvwr_params.h
   dbdet_dlvwr.cxx                dbdet_dlvwr.h
   dbdet_dlvwr_tool.cxx           dbdet_dlvwr_tool.h
   dbdet_dmedial_wire_tool.cxx    dbdet_dmedial_wire_tool.h
   dbacm_levelset_tool.cxx        dbacm_levelset_tool.h
   dbdet_image_seg_tool.cxx       dbdet_image_seg_tool.h
   dbdet_manual_seg_tool.cxx       dbdet_manual_seg_tool.h
   dbdet_image_seg_dlvwr_tool.cxx dbdet_image_seg_dlvwr_tool.h
   # dbdet_load_volume_seg_xml_process.cxx    dbdet_load_volume_seg_xml_process.h
   
   bvis1_dicom_image_displayer.cxx   bvis1_dicom_image_displayer.h
   dbknee_vsol2D_displayer.cxx       dbknee_vsol2D_displayer.h
   
)


AUX_SOURCE_DIRECTORY(Templates medial_livewire_gui_sources)

ADD_EXECUTABLE(medial_livewire_gui ${medial_livewire_gui_sources})

TARGET_LINK_LIBRARIES(medial_livewire_gui bvis1 dbgui dbsol vidpro1 vidl1 dbdet_vis dbdet_xio dbsol_vis dbsol_pro)
#TARGET_LINK_LIBRARIES(medial_livewire_gui bvis1 dbgui dbsol vidpro1 vidl1 dbdet_vis dbdet_xio dbmsh3d_algovtk_pro dbsol_vis dbsol_pro )


ENDIF (VXL_VGUI_FOUND)
